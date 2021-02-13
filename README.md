# Pulse-oximeter using MAX30102 <BR>パルスオキシメーター MAX30102 使用

## LCD 表示
<IMG alt=LCD src="img/lcd01.png"> <IMG alt=MAX30102 src="img/max30102.png">
<BR><BR><BR>

## Schematic 回路図
<IMG alt=schematic src="img/schematic.png">
<BR>

## PCB 基板
Gerber files are in preparation. ガーバーファイルは準備中です。<BR>
<IMG alt=schematic src="img/pcb.png">
<BR>

## BOM 部品表
Under construction. 準備中
<BR><BR><BR>

## SpO<sub>2</sub> calculation 計算式
Four options are implemented. Default is option 3.<BR>
4つあります。既定では3です。<BR>
  1. SpO<sub>2</sub> = -33.437 <i>R</i> + 114.9 (linear approximation of Reynolds et al. (1991) Br J Anaesth 67, 638)<BR>
  2. SpO<sub>2</sub> = -17 <i>R</i> + 104 (User guides & manuals 6409, Maxim Integrated)<BR>
  3. SpO<sub>2</sub> = 1.5958422 <i>R</i><sup>2</sup> -34.6596622 <i>R</i> + 112.6898759 (Application note 6845, Maxim Integrated)<BR>
  4. SpO<sub>2</sub> = -45.060 <i>R</i><sup>2</sup> + 30.354 <i>R</i> + 94.845 (MAX3010x sensor library, SparkFun)<BR>
where <i>R</i> = (AC<sub>red</sub> / DC<sub>red</sub>) / (AC<sub>ir</sub> / DC<sub>ir</sub>).<BR>
<IMG alt=spo2models src="img/spo2plot.png">
<BR>

## References 参考
Reynolds et al. (1991) Br J Anaesth 67, 638-643. <a href="https://doi.org/10.1093/bja/67.5.638">DOI</A><BR>
Recommended Configurations and Operating Profiles for MAX30101/MAX30102 EV Kits, <a href="https://www.maximintegrated.com/en/design/technical-documents/userguides-and-manuals/6/6409.html">User guides & manuals 6409</a>, Maxim Integrated<BR>
Guidelines for SpO2 Measurement Using the MaximR MAX32664 Sensor Hub, <a href="https://www.maximintegrated.com/en/design/technical-documents/app-notes/6/6845.html">Application note 6845</a>, Maxim Integrated<BR>
MAX3010x sensor library, SparkFun. <a href="https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library">GitHub</a><BR>

## DISCLAIMER お断り
This repository is provided "AS IS", without warranty of any kind, either express or implied. このリポジトリはこのままの形で提供されており、明示・暗示を問わず、いかなる保証も致しません。
