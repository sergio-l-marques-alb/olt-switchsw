pll_init cxo2t4_pll_156M_NIFs.cfg

#Tira de RST o Controlo da Ventilacao
sb 8000FF10h 1 1
sb 8000FF11h 1 1

sh bcm.init.sh

