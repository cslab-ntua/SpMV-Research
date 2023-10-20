Every time new measurements are added here, they have to be filtered first. 

Regarding "validation" matrices:
From the csv files with the collected benchmarks, we want to combine them in one common final csv file. Towards this, use the bash script in the "parse_cpu_validation" directory, editing appropriately for the desired device.


Regarding "synthetic" and "friends" matrices:
Filtering/editing has to be applied first. This happens because SELL-C-s is executed through a different project from others.
Therefore, slight differences in feature values occur, and the grouping of executions by features is not perfect (when extracting best performance for each matrix tested in "synthetic" and "friends" datasets).
What we have to do is move the benchmark results in 'fix_intermediate_results' and use the notebook, using the appropriate "feats_synthetic" or "feats_friends" csv file.
The structure of an example directory in there has to be:

intel-icy3
	|
	-> synthetic1 (original csv files)
	-> synthetic_new1 (where edited csv files will be stored)
