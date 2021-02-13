# A pulse-oximeter using MAX30102 <BR>パルスオキシメーター　MAX30102使用

## LCD
<IMG alt=LCD src="img/lcd01.png"> <IMG alt=MAX30102 src="img/max30102.png">

## Schematic
<IMG alt=schematic src="img/schematic.png">

## PCB
<IMG alt=schematic src="img/pcb.png">

## BOM
under construction

## SpO<sub>2</sub> calculation
Four options are implemented. Our recommendation is option 1.<BR>
4つあります。1がお勧めです。<BR>
  1. SpO<sub>2</sub> = 1.5958422 <i>R</i><sup>2</sup> -34.6596622 <i>R</i> + 112.6898759 (Application note 6845, Maxim Integrated)<BR>
  2. SpO<sub>2</sub> = -33.437 <i>R</i> + 114.9 (linear approximation of Reynolds et al. (1991) Br J Anaesth 67, 638)<BR>
  3. SpO<sub>2</sub> = -17 <i>R</i> + 104 (UG6409, Maxim Integrated)<BR>
  4. SpO<sub>2</sub> = -45.060 <i>R</i><sup>2</sup> + 30.354 <i>R</i> + 94.845 (MAX3010x sensor library, SparkFun)<BR>
where <i>R</i> = (AC<sub>red</sub> / DC<sub>red</sub>) / (AC<sub>ir</sub> / DC<sub>ir</sub>).<BR>
<IMG alt=spo2models src="img/spo2plot.png">

## References
Reynolds et al. (1991) Br J Anaesth 67, 638-643<BR>
Guidelines for SpO2 Measurement Using the MaximR MAX32664 Sensor Hub, Application note 6845, Maxim Integrated<BR>
Recommended Configurations and Operating Profiles for MAX30101/MAX30102 EV Kits, UG6409, Maxim Integrated<BR>
MAX3010x sensor library, SparkFun<BR>
