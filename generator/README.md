# Event Generator Package



## gen_pizero
```
cd tests
gen_piZero.py --nov 2 -o Pi0.EVT.root &> pi_zero.log&
```


```
simu_trf.py -i Pi02.EVT.root -o Pi02.HIT.root -nt 2
digit_trf.py -i Pi02.HIT.root -o Pi02.ESD.root
reco_trf.py -i Pi02.ESD.root -o Pi02.AOD.root
```

simu_trf.py -i Pi0.EVT.root -o Pi0.HIT.root -nt 2
