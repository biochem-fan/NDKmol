NDKmol - Molecular Viewer on Android NDK

== About ==

NDKmol is a molecular viewer for Android.
You can view three dimensional structures of proteins and
small molecules. NDKmol supports most of common representations
for molecules, such as ribbon, trace, stick, sphere and line.
NDKmol also supports symmetry operations; biological assemblies and
crystal packing can be displayed.

NDKmol is very compact and optimized for performance.
You can open large molecules like ribosome!

NDKmol has same functionality as GLmol, which is written in
WebGL/Javascript and runs on Web browsers.
You can try GLmol at 
http://webglmol.sourceforge.jp/index-en.html

== Features ==

* Read PDB file
* Read SDF/MOL file
* Search and Download structures from RCSB PDB and NCBI PubChem
* Rotate/Translate/Zoom model by finger
* Representations
    - Line
    - Stick
    - Sphere(van der Waals radius)
    - Alpha carbon trace
    - Ribbon (thick or thin)
    - Strand
    - B factor tube
    - Nucleic acid ladder
    - Nucleic acid line
    - Solvent 'stars'
* Smoothing of beta sheets
* Coloring
    - By chain
    - By secondary structure(when defined in SHEET/HELIX records)
    - By Elements
    - Gradation (a.k.a chainbow)
    - B factor
    - polar/nonpolar
* Crystallography
    - Display unit cell
    - Show crystal packing (when defined in REMARK section)
    - Display biological assembly (when defined in REMARK section)

== How to Use ==

When launched, NDKmol automatically loads porin (PDBID: 2POR)
as an example. You can rotate the molecule by your finger.

To zoom or translate the molecule, press <MENU> button in your
phone/tablet and select the mode. Two-finger gestures are
also supported.

Everything can be done from the <MENU>; changing representation
of protein or ligand (non protein molecules), changing colors,
showing biological assembly, crystal packing and/or unit cell.

To load other PDB files, please put the file in "PDB" directory 
of the SD card and select "Open" command in the MENU.
You can also download structures directly from RCSB PDB and NCBI PubChem
web server. Select "Search and Download" in the menu.

== Reference ==

A PDB file (PDBID: 2POR) is included as an example.
 "Structure of porin refined at 1.8 A resolution"
   Weiss, M.S.,  Schulz, G.E., J.Mol.Biol. 227: 493-509 (1992)

== Contact ==

Project website is located at http://webglmol.sourceforge.jp/

Comments and suggestions are welcome at http://sourceforge.jp/projects/webglmol/forums/ or 
biochem_fan@users.sourceforge.jp 
