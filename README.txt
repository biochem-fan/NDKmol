Molecular Viewer for iOS

How to use:

 single-finger drag - rotation
 double-finger drag(pan) - translation
 double-finger pinch - zoom
 single-finger long press - open "view settings" panel

Features:

 PDB/SDF/MOL parser and graphics engine have same capabilities
 as NDKmol.
 File browser and PDB/PubChem searcher is not implemented.

How to integrate to your code:

 "MolecularViewerViewController.mm" is the main view controller.
 You can invoke this controller from your code.

 To change models, save PDB file in the resource directory and
 call "loadMolecule" method in MolecularViewerViewController.mm

License:

 (C) Copyright biochem_fan, 2011-2012

 LGPL version 3.
 Briefly, you don't have to open-source your code but if you
 modified my code, please open-source the modification so that I can merge it.
 Also please acknowledge use of my code in e.g. "About" dialog.

 If LGPL is not suitable for your purpose, please tell me. 
 I will consider another license.
