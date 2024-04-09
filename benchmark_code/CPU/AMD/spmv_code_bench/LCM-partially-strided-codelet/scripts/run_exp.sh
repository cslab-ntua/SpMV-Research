

BINLIB=$1
PATHMAIN=$2
TUNED=$3
THRDS=$4


export OMP_NUM_THREADS=$THRDS
export MKL_NUM_THREADS=$THRDS

header=1
#MATS="af_0_k101.mtx BenElechi1.mtx ecology2.mtx hood.mtx nd24k.mtx thermomech_dM.mtx af_shell10.mtx bmwcra_1.mtx Emilia_923.mtx Hook_1498.mtx parabolic_fem.mtx tmt_sym.mtx af_shell7.mtx bone010.mtx Fault_639.mtx ldoor.mtx PFlow_742.mtx apache2.mtx boneS10.mtx Flan_1565.mtx msdoor.mtx StocF-1465.mtx audikw_1.mtx crankseg_2.mtx G3_circuit.mtx nd12k.mtx thermal2.mtx bundle_adj.mtx pwtk.mtx m_t1.mtx x104.mtx consph.mtx shipsec5.mtx thread.mtx s3dkq4m2.mtx pdb1HYS.mtx offshore.mtx cant.mtx smt.mtx Dubcova3.mtx cfd2.mtx nasasrb.mtx ct20stif.mtx vanbody.mtx oilpan.mtx qa8fm.mtx 2cubes_sphere.mtx raefsky4.mtx msc10848.mtx denormal.mtx bcsstk36.mtx gyro.mtx olafu.mtx Pres_Poisson.mtx bundle1.mtx cbuckle.mtx fv2.mtx msc23052.mtx aft01.mtx Muu.mtx Kuu.mtx obstclae.mtx nasa2910.mtx s3rmt3m3.mtx bcsstk16.mtx Trefethen_20000.mtx bcsstk24.mtx ted_B_unscaled.mtx minsurfo.mtx"
#MATS="gyro_k.mtx Dubcova2.mtx msc23052.mtx Pres_Poisson.mtx cbuckle.mtx thermomech_dM.mtx olafu.mtx Dubcova3.mtx parabolic_fem.mtx ecology2.mtx gyro.mtx raefsky4.mtx"

if [ "$TUNED" ==  1 ]; then
for bp in {0,1}; do
for mat in $PATHMAIN/*.mtx; do
for k in {2,3,4,5,6,7,8,9}; do
#	for cparm in {1,2,3,4,5,10,20}; do
	if [ $header -eq 1 ]; then
	  $BINLIB  -m $mat -n SPTRS -s CSR -t $THRDS -c $k -d -p $bp -u 1
     header=0
  else
	  $BINLIB  -m $mat -n SPTRS -s CSR -t $THRDS -c $k -p $bp -u 1
  fi
#done
done
done

#lfactors
for mat in $PATHMAIN/l_factors/*.mtx; do
for k in {2,3,4,5,6,7,8,9}; do
#for lparm in {1..10}; do
#	for cparm in {1,2,3,4,5,10,20}; do
	  $BINLIB  -m $mat -n SPTRS -s CSR -t $THRDS -c $k -p $bp -u 1
#done
#done
done
done
done
fi


if [ "$TUNED" == 2 ]; then
for mat in $PATHMAIN/*.mtx; do
	if [ $header -eq 1 ]; then
	  $BINLIB  -m $mat -n SPTRS -s CSR -t $THRDS -d
     header=0
  else
	  $BINLIB  -m $mat -n SPTRS -s CSR -t $THRDS
  fi
done

#Lfactors
for mat in $PATHMAIN/l_factors/*.mtx; do
	  $BINLIB  -m $mat -n SPTRS -s CSR -t $THRDS
done

fi


### SPMV
if [ "$TUNED" == 3 ]; then
for mat in $PATHMAIN/*.mtx; do
	if [ $header -eq 1 ]; then
	  $BINLIB  -m $mat -n SPMV -s CSR -t $THRDS -d
     header=0
  else
	  $BINLIB  -m $mat -n SPMV -s CSR -t $THRDS
  fi
done

#Lfactors
for mat in $PATHMAIN/l_factors/*.mtx; do
	  $BINLIB  -m $mat -n SPMV -s CSR -t $THRDS
done

fi
