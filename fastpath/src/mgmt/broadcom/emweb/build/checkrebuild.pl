#	-cmp $(OD)/content.gen $(OD)/temp.gen
#	if not ERRORLEVEL 0 (
#	cp -f $(OD)/temp.gen $(OD)/content.gen
#	touch ../content/index.html

$od = shift(@ARGV);
$ewc = shift(@ARGV);
$pkg = shift(@ARGV);
$dbg = shift(@ARGV);

$con = "_content.gen";
$tmp = "_temp.gen";
$inx = "index.html";

print("cmp -s $od/$con $od/$tmp\n") unless $dbg;
$cmp = system("cmp -s $od/$con $od/$tmp")/256;
if ($cmp ne '0')
{
   print("GENERATING...[$pkg]: $con\n");
   print("cp -f $od/$tmp $od/$con\n") unless $dbg;
   `cp -f $od/$tmp $od/$con`;
   print("rm -f $ewc/root/$inx\n") unless $dbg;
   `rm -f $ewc/root/$inx`;
   print("cp -p $ewc/../build/$inx $ewc/root/$inx\n") unless $dbg;
   `cp -p $ewc/../build/$inx $ewc/root/$inx`;
   print("touch $ewc/root/$inx\n") unless $dbg;
   `touch $ewc/root/$inx`;
}
