<?php
$handle = fopen('php://stdin', 'r');
if ($handle) {
	while (($buffer = fgets($handle, 4096)) !== false) {
		$tmp = explode("\n",$buffer);
		foreach ($tmp as $t) {
			if (preg_match("/(Cycle 2)/i", $t)) {
				$tmp2 = explode(",",$t);
				$kwh = floatval(trim($tmp2[1]));
				echo "New kWh = ". $kwh . "\n";
				exec("echo \"update kWh.rrd N:". $kwh . "\" | rrdtool -");
			}
		}
	}
	fclose($handle);
}
?>
