#!/bin/bash
echo '"Good" runs:'
echo "- OpenMP"
for cpt in 1 2 4; do
    srun --nodes=1 --ntasks-per-node=1 --cpus-per-task=${cpt} --threads-per-core=1 ./pi_omp
done

echo "- MPI"
npn_cpt_list="1,1 1,2 1,4 2,1 2,2 4,1"
for nodes in 1 2; do
    for npn_cpt in npn_cpt_list; do
        npn=$(echo ${npn_cpt} | cut -d, -f1)
        cpt=$(echo ${npn_cpt} | cut -d, -f2)
        srun --nodes=${nodes} --ntasks-per-node=${npn} --cpus-per-task=${cpt} --threads-per-core=1 ./pi_omp_mpi
    done
done

echo '"Bad" runs:'
echo "- OpenMP"
for cpt in 1 2 4; do
    srun --nodes=1 --ntasks-per-node=1 --cpus-per-task=${cpt} --threads-per-core=2 ./pi_omp
done
