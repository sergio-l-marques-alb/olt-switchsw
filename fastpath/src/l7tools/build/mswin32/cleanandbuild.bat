if exist %FP_SRC%/extract.log rm %FP_SRC%/extract.log

perl %FP_SRC%/l7tools/build/codemake.pl all clean
perl %FP_SRC%/l7tools/build/codemake.pl all bldlib
perl %FP_SRC%/l7tools/build/codemake.pl all installexe
perl %FP_SRC%/l7tools/build/clean_up.pl

if exist %FP_HWSRC%/ipl/bootos echo Build Successful! >> %FP_SRC%/extract.log
if not exist %FP_HWSRC%/ipl/bootos perl %FP_SRC%/l7tools/build/snafu.pl %FP_SRC% >> %FP_SRC%/extract.log
if not exist %FP_HWSRC%/ipl/bootos perl %FP_SRC%/l7tools/build/snafu.pl %FP_HWSRC% >> %FP_SRC%/extract.log
cls
cat %FP_SRC%/extract.log
