#include "TBetterChain.h"

TBetterChain::TBetterChain():
TChain()
{}

TBetterChain::TBetterChain(const char* name, const char* title):
  TChain(name, title)
{}

TBetterChain::~TBetterChain(){}

Int_t TBetterChain::GetEntry(Long64_t entry, Int_t getall)
{
  Long64_t treeReadEntry = LoadTree(entry);
  if (treeReadEntry < 0) {
    return 0;
  }
  if (!fTree) {
    return 0;
  }
  return fTree->GetEntry(treeReadEntry, getall);
}

Long64_t TBetterChain::LoadTree(Long64_t entry)
{
  // We already have been visited while recursively looking
  // through the friends tree, let's return.
  if (kLoadTree & fFriendLockStatus) {
    return 0;
  }

  if (!fNtrees) {
    // -- The chain is empty.
    return -1;
  }

  if ((entry < 0) || ((entry > 0) && (entry >= fEntries && entry!=(TTree::kMaxEntries-1) ))) {
    // -- Invalid entry number.
    if (fTree) fTree->LoadTree(-1);
    fReadEntry = -1;
    return -2;
  }

  // Find out which tree in the chain contains the passed entry.
  Int_t treenum = fTreeNumber;
  if ((fTreeNumber == -1) || (entry < fTreeOffset[fTreeNumber]) || (entry >= fTreeOffset[fTreeNumber+1]) || (entry==TTree::kMaxEntries-1)) {
    // -- Entry is *not* in the chain's current tree.
    // Do a linear search of the tree offset array.
    // FIXME: We could be smarter by starting at the
    //        current tree number and going forwards,
    //        then wrapping around at the end.
    for (treenum = 0; treenum < fNtrees; treenum++) {
      if (entry < fTreeOffset[treenum+1]) {
	break;
      }
    }
  }

  // Calculate the entry number relative to the found tree.
  Long64_t treeReadEntry = entry - fTreeOffset[treenum];
  fReadEntry = entry;

  // If entry belongs to the current tree return entry.
  if (fTree && treenum == fTreeNumber) {
    // First set the entry the tree on its owns friends
    // (the friends of the chain will be updated in the
    // next loop).
    assert(fTree->LoadTree(treeReadEntry)>=0);
    if (fFriends) {
      // The current tree has not changed but some of its friends might.
      //
      // An alternative would move this code to each of
      // the functions calling LoadTree (and to overload a few more).
      TIter next(fFriends);
      TFriendLock lock(this, kLoadTree);
      TFriendElement* fe = 0;
      TFriendElement* fetree = 0;
      Bool_t needUpdate = kFALSE;
      while ((fe = (TFriendElement*) next())) {
	TObjLink* lnk = 0;
	if (fTree->GetListOfFriends()) {
	  lnk = fTree->GetListOfFriends()->FirstLink();
	}
	fetree = 0;
	while (lnk) {
	  TObject* obj = lnk->GetObject();
	  if (obj->TestBit(TFriendElement::kFromChain) && obj->GetName() && !strcmp(fe->GetName(), obj->GetName())) {
	    fetree = (TFriendElement*) obj;
	    break;
	  }
	  lnk = lnk->Next();
	}
	TTree* at = fe->GetTree();
	if (at->InheritsFrom(TChain::Class())) {
	  Int_t oldNumber = ((TChain*) at)->GetTreeNumber();
	  TTree* old = at->GetTree();
	  TTree* oldintree = fetree ? fetree->GetTree() : 0;
	  assert(at->LoadTreeFriend(entry, this)>=0);
	  Int_t newNumber = ((TChain*) at)->GetTreeNumber();
	  if ((oldNumber != newNumber) || (old != at->GetTree()) || (oldintree && (oldintree != at->GetTree()))) {
	    // We can not compare just the tree pointers because
	    // they could be reused. So we compare the tree
	    // number instead.
	    needUpdate = kTRUE;
	    fTree->RemoveFriend(oldintree);
	    fTree->AddFriend(at->GetTree(), fe->GetName())->SetBit(TFriendElement::kFromChain);
	  }
	} else {
	  // else we assume it is a simple tree If the tree is a
	  // direct friend of the chain, it should be scanned
	  // used the chain entry number and NOT the tree entry
	  // number (treeReadEntry) hence we redo:
	  assert(at->LoadTreeFriend(entry, this)>=0);
	}
      }
      if (needUpdate) {
	// Update the branch/leaf addresses and
	// thelist of leaves in all TTreeFormula of the TTreePlayer (if any).

	// Set the branch statuses for the newly opened file.
	TChainElement *frelement;
	TIter fnext(fStatus);
	while ((frelement = (TChainElement*) fnext())) {
	  Int_t status = frelement->GetStatus();
	  fTree->SetBranchStatus(frelement->GetName(), status);
	}

	// Set the branch addresses for the newly opened file.
	fnext.Reset();
	while ((frelement = (TChainElement*) fnext())) {
	  void* addr = frelement->GetBaddress();
	  if (addr) {
	    TBranch* br = fTree->GetBranch(frelement->GetName());
	    TBranch** pp = frelement->GetBranchPtr();
	    if (pp) {
	      // FIXME: What if br is zero here?
	      *pp = br;
	    }
	    if (br) {
	      // FIXME: We may have to tell the branch it should
	      //        not be an owner of the object pointed at.
	      br->SetAddress(addr);
	      if (TestBit(kAutoDelete)) {
		br->SetAutoDelete(kTRUE);
	      }
	    }
	  }
	}
	if (fPlayer) {
	  fPlayer->UpdateFormulaLeaves();
	}
	// Notify user if requested.
	if (fNotify) {
	  fNotify->Notify();
	}
      }
    }
    return treeReadEntry;
  }

  // Delete the current tree and open the new tree.

  TTreeCache* tpf = 0;
  // Delete file unless the file owns this chain!
  // FIXME: The "unless" case here causes us to leak memory.
  if (fFile) {
    if (!fDirectory->GetList()->FindObject(this)) {
      if (fTree) {
	// (fFile != 0 && fTree == 0) can happen when
	// InvalidateCurrentTree is called (for example from
	// AddFriend).  Having fTree === 0 is necessary in that
	// case because in some cases GetTree is used as a check
	// to see if a TTree is already loaded.
	// However, this prevent using the following to reuse
	// the TTreeCache object.
	tpf = (TTreeCache*) fFile->GetCacheRead(fTree);
	if (tpf) {
	  tpf->ResetCache();
	}

	fFile->SetCacheRead(0, fTree);
	// If the tree has clones, copy them into the chain
	// clone list so we can change their branch addresses
	// when necessary.
	//
	// This is to support the syntax:
	//
	//      TTree* clone = chain->GetTree()->CloneTree(0);
	//
	// We need to call the invalidate exactly here, since
	// we no longer need the value of fTree and it is
	// about to be deleted.
	InvalidateCurrentTree();
      }

      if (fCanDeleteRefs) {
	fFile->Close("R");
      }
      delete fFile;
      fFile = 0;
    } else {
      // If the tree has clones, copy them into the chain
      // clone list so we can change their branch addresses
      // when necessary.
      //
      // This is to support the syntax:
      //
      //      TTree* clone = chain->GetTree()->CloneTree(0);
      //
      if (fTree) InvalidateCurrentTree();
    }
  }

  TChainElement* element = (TChainElement*) fFiles->At(treenum);
  if (!element) {
    if (treeReadEntry) {
      return -4;
    }
    // Last attempt, just in case all trees in the chain have 0 entries.
    element = (TChainElement*) fFiles->At(0);
    if (!element) {
      return -4;
    }
  }

  // FIXME: We leak memory here, we've just lost the open file
  //        if we did not delete it above.
  {
    TDirectory::TContext ctxt;
    fFile = TFile::Open(element->GetTitle());
    if (fFile) fFile->SetBit(kMustCleanup);
  }

  // ----- Begin of modifications by MvL
  Int_t returnCode = 0;
  if (!fFile || fFile->IsZombie()) {
    if (fFile) {
      delete fFile;
      fFile = 0;
    }
    // Note: We do *not* own fTree.
    fTree = 0;
    returnCode = -3;
  } else {
    // Note: We do *not* own fTree after this, the file does!
    fTree = (TTree*) fFile->Get(element->GetName());
    if (!fTree) {
      // Now that we do not check during the addition, we need to check here!
      Error("LoadTree", "Cannot find tree with name %s in file %s", element->GetName(), element->GetTitle());
      delete fFile;
      fFile = 0;
      // We do not return yet so that 'fEntries' can be updated with the
      // sum of the entries of all the other trees.
      returnCode = -4;
    }
  }

  fTreeNumber = treenum;
  // FIXME: We own fFile, we must be careful giving away a pointer to it!
  // FIXME: We may set fDirectory to zero here!
  fDirectory = fFile;

  // Reuse cache from previous file (if any).
  if (tpf) {
    if (fFile) {
      tpf->ResetCache();
      fFile->SetCacheRead(tpf, fTree);
      // FIXME: fTree may be zero here.
      tpf->UpdateBranches(fTree);
    } else {
      // FIXME: One of the file in the chain is missing
      // we have no place to hold the pointer to the
      // TTreeCache.
      delete tpf;
      tpf = 0;
    }
  } else {
    if (fCacheUserSet) {
      this->SetCacheSize(fCacheSize);
    }
  }

  // Check if fTreeOffset has really been set.
  Long64_t nentries = 0;
  if (fTree) {
    nentries = fTree->GetEntries();
  }

  if (fTreeOffset[fTreeNumber+1] != (fTreeOffset[fTreeNumber] + nentries)) {
    fTreeOffset[fTreeNumber+1] = fTreeOffset[fTreeNumber] + nentries;
    fEntries = fTreeOffset[fNtrees];
    element->SetNumberEntries(nentries);
    // Below we must test >= in case the tree has no entries.
    if (entry >= fTreeOffset[fTreeNumber+1]) {
      if ((fTreeNumber < (fNtrees - 1)) && (entry < fTreeOffset[fTreeNumber+2])) {
	// The request entry is not in the tree 'fTreeNumber' we will need
	// to look further.

	// Before moving on, let's record the result.
	element->SetLoadResult(returnCode);

	// Before trying to read the file file/tree, notify the user
	// that we have switched trees if requested; the user might need
	// to properly account for the number of files/trees even if they
	// have no entries.
	if (fNotify) {
	  fNotify->Notify();
	}

	// Load the next TTree.
	return LoadTree(entry);
      } else {
	treeReadEntry = fReadEntry = -2;
      }
    }
  }


  if (!fTree) {
    // The Error message already issued.  However if we reach here
    // we need to make sure that we do not use fTree.
    //
    // Force a reload of the tree next time.
    fTreeNumber = -1;

    element->SetLoadResult(returnCode);
    return returnCode;
  }
  // ----- End of modifications by MvL

  // Copy the chain's clone list into the new tree's
  // clone list so that branch addresses stay synchronized.
  if (fClones) {
    for (TObjLink* lnk = fClones->FirstLink(); lnk; lnk = lnk->Next()) {
      TTree* clone = (TTree*) lnk->GetObject();
      ((TChain*) fTree)->TTree::AddClone(clone);
    }
  }

  // Since some of the friends of this chain might simple trees
  // (i.e., not really chains at all), we need to execute this
  // before calling LoadTree(entry) on the friends (so that
  // they use the correct read entry number).

  // Change the new current tree to the new entry.
  Long64_t loadResult = fTree->LoadTree(treeReadEntry);
  if (loadResult == treeReadEntry) {
    element->SetLoadResult(0);
  } else {
    // This is likely to be an internal error, if treeReadEntry was not in range
    // (or intentionally -2 for TChain::GetEntries) then something happened
    // that is very odd/surprising.
    element->SetLoadResult(-5);
  }


  // Change the chain friends to the new entry.
  if (fFriends) {
    // An alternative would move this code to each of the function
    // calling LoadTree (and to overload a few more).
    TIter next(fFriends);
    TFriendLock lock(this, kLoadTree);
    TFriendElement* fe = 0;
    while ((fe = (TFriendElement*) next())) {
      TTree* t = fe->GetTree();
      if (!t) continue;
      if (t->GetTreeIndex()) {
	t->GetTreeIndex()->UpdateFormulaLeaves(0);
      }
      if (t->GetTree() && t->GetTree()->GetTreeIndex()) {
	t->GetTree()->GetTreeIndex()->UpdateFormulaLeaves(GetTree());
      }
      assert(t->LoadTreeFriend(entry, this)>=0);
      TTree* friend_t = t->GetTree();
      if (friend_t) {
	fTree->AddFriend(friend_t, fe->GetName())->SetBit(TFriendElement::kFromChain);
      }
    }
  }

  fTree->SetMakeClass(fMakeClass);
  fTree->SetMaxVirtualSize(fMaxVirtualSize);

  SetChainOffset(fTreeOffset[fTreeNumber]);

  // Set the branch statuses for the newly opened file.
  TIter next(fStatus);
  while ((element = (TChainElement*) next())) {
    Int_t status = element->GetStatus();
    fTree->SetBranchStatus(element->GetName(), status);
  }

  // Set the branch addresses for the newly opened file.
  next.Reset();
  while ((element = (TChainElement*) next())) {
    void* addr = element->GetBaddress();
    if (addr) {
      TBranch* br = fTree->GetBranch(element->GetName());
      TBranch** pp = element->GetBranchPtr();
      if (pp) {
	// FIXME: What if br is zero here?
	*pp = br;
      }
      if (br) {
	// FIXME: We may have to tell the branch it should
	//        not be an owner of the object pointed at.
	br->SetAddress(addr);
	if (TestBit(kAutoDelete)) {
	  br->SetAutoDelete(kTRUE);
	}
      }
    }
  }

  // Update the addresses of the chain's cloned trees, if any.
  if (fClones) {
    for (TObjLink* lnk = fClones->FirstLink(); lnk; lnk = lnk->Next()) {
      TTree* clone = (TTree*) lnk->GetObject();
      CopyAddresses(clone);
    }
  }

  // Update list of leaves in all TTreeFormula's of the TTreePlayer (if any).
  if (fPlayer) {
    fPlayer->UpdateFormulaLeaves();
  }

  // Notify user we have switched trees if requested.
  if (fNotify) {
    fNotify->Notify();
  }

  // Return the new local entry number.
  return treeReadEntry;
}
