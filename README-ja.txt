NDKmol - Molecular Viewer on Android NDK

== 概要 ==

NDKmol は Android 用の分子構造ビューアです。蛋白質や低分子の立体構造を表示できます。

NDKmol は軽快に動くように工夫されており、リボソームのような巨大分子を表示することも可能です。

NDKmol は WebGL/Javascript ベースの分子構造ビューアである GLmol と同等の機能を
持っています。GLmol については
http://webglmol.sourceforge.jp/index-ja.html
をご覧ください。

== 機能 ==

現在、以下の機能があります。他にも多くの機能が開発中です。

* PDB ファイルの読み込み
* SDF/MOL ファイルの読み込み
* RCSB PDB または NCBI PubChem から構造データを検索・ダウンロード 
* タッチ操作による分子の回転・平行移動・拡大縮小
* 各種表示方法
   -  線
   -  棒
   -  球(ファンデルワールス半径)
   -  主鎖トレース
   -  リボン(厚みあり、なし)
   -  寄り紐(ストランド)
   -  温度因子チューブ
   -  核酸 ラダー
   -  核酸 線
   -  溶媒
* ベータシートの平滑化
* 各種着色
   -  鎖ごと
   -  二次構造 (SHEET/HELIXレコードに記載がある場合)
   -  元素
   -  グラデーション
   -  温度因子
   -  極性・非極性
* 結晶学
   -  単位胞表示
   -  パッキングの表示 (REMARKセクションに記載がある場合)
   -  生物学的多量体(biological assembly)の表示 (REMARKセクションに記載がある場合) 

== 使用方法 ==

起動すると自動的にポリン (PDBID: 2POR) を表示します。
画面を指で触ることで回転できます。また、二本指によるピンチズームや移動にも対応しています。
分子を拡大縮小したり移動するには、メニューボタンを押してモードを切り替えてください。

表示方法や色の切り替え、多量体やパッキングの表示など、全ての操作はメニューから行います。

他の PDB ファイルを読み込むには、PDBファイルを SDカード内の "PDB"フォルダに配置し、
メニューの "Open" から読み込んでください。
RCSB PDB や NCBI PubChem のサーバから直接構造を検索してダウンロードすることもできます。
メニューから "検索してダウンロード" を選択してください。

== 参考文献 ==

サンプルとして同梱してあるポリンの構造 (PDBID: 2POR): 
 "Structure of porin refined at 1.8 A resolution"
   Weiss, M.S.,  Schulz, G.E., J.Mol.Biol. 227: 493-509 (1992)

== ライセンス ==

LGPL version 3 とします。詳細は LICENSE.txt をご覧ください。

== 連絡先 ==

ご意見・ご感想は、 http://sourceforge.jp/projects/webglmol/forums/ または
biochem_fan@users.sourceforge.jp までお気軽にどうぞ。
