# Event Generator Package



## gen_pizero
```
cd tests
gen_piZero.py --nov 2 -o Pi0.EVT.root &> pi_zero.log&
```


```
simu_trf.py -i Pi0.EVT.root -o Pi0.HIT.root -nt 2
digit_trf.py -i Pi0.HIT.root -o Pi0.ESD.root
reco_trf.py -i Pi0.ESD.root -o Pi0.AOD.root
```

This is for testing when not using the container
gen_piZero.py --nov 2 -o Pi02.EVT.root &> pi_zero2.log&
simu_trf.py -i Pi02.EVT.root -o Pi02.HIT.root -nt 2
digit_trf.py -i Pi02.HIT.root -o Pi02.ESD.root
reco_trf.py -i Pi02.ESD.root -o Pi02.AOD.root