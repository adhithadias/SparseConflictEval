!#/bin/bash

# python scripts/histogram.py bcsstk17 \
#     scircuit mac_econ_fwd500 majorbasis Lin \
#     rma10 cop20k_A webbase-1M cant pdb1HYS ecology1 \
#     largebasis consph \
#     shipsec1 atmosmodd pwtk

python scripts/histogram.py bcsstk17 \
    scircuit mac_econ_fwd500 majorbasis Lin \
    ecology1 largebasis atmosmodd pwtk \
    rma10 cop20k_A webbase-1M cant pdb1HYS \
    consph shipsec1

python scripts/histogram_tns.py vast-2015-mc1-3d \
    darpa1998 nell-2 freebase_music flickr-3d \
    freebase_sampled nell-1 