#!/bin/bash
echo '"Good" runs:'
echo "- OpenMP"
for cpt in 1 2 4; do
    srun --nodes=1 --ntasks-per-node=1 --cpus-per-task=${cpt} --threads-per-core=1 ./pi_omp
done

echo "- MPI"
tpn_cpt_list="1,1 1,2 1,4 2,1 2,2 4,1"
for nodes in 1 2; do
    for tpn_cpt in ${tpn_cpt_list}; do
        tpn=$(echo ${tpn_cpt} | cut -d, -f1)
        cpt=$(echo ${tpn_cpt} | cut -d, -f2)
        srun --nodes=${nodes} --ntasks-per-node=${tpn} --cpus-per-task=${cpt} --threads-per-core=1 ./pi_omp_mpi
    done
done

echo '"Bad" runs:'
echo "- OpenMP"
for cpt in 1 2 4 8; do
    srun --nodes=1 --ntasks-per-node=1 --cpus-per-task=${cpt} --threads-per-core=2 ./pi_omp
done
