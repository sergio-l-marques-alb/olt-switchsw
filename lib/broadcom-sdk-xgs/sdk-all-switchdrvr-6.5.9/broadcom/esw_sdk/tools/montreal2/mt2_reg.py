#!/usr/bin/env python
import os, sys, getopt

def sbus_reg(_base_addr, _block, _sys, _port):

    _stage = (_base_addr & 0xfc000000) >> 26
    if (_stage & ~0xf) :
        print 'Not enough bits to represent stage_id'
        return

    if (_block & ~0xf) :
        print 'Not enough bits to represent block'
        return

    _port = (_port & 0x3) + ((_sys & 0x1) << 2)

    if _base_addr & (1 << 25):
        print 'Per quad register block = %d' % (_block)
    else:
        print 'Per port register block = %d  port = %d' % (_block , _port)

    print "reg=0x%08x" % ( ((_base_addr & 0x3ffff00) << 6) |  ( (_port & 0xf) << 10) | ((_stage & 0xf) << 6) |((_block & 0xf) << 2) | 0x3 )


def totsc_reg(_base_addr, _sys, _port, _lane_mode, _pmd):
    _int_port = (((_sys) << 3) + (_port & 0x7)  + 1)
    print "reg=0x%08x" % ( ((_base_addr & 0xffff) << 6) |  ( (_int_port & 0x1f) << 25) | ((_lane_mode & 0x7) << 22) | (_pmd + 0x1) )

def axi_reg(_base_addr):
    print "reg=0x%08x" % ( _base_addr & ~0x3 )


if __name__ == '__main__':
   type = "axi"
   block = 0
   pmd = 0
   port = 0
   sys_side = 0
   lane_mode = 0
   addr = 0x0

   try:
      opts, args = getopt.getopt(sys.argv[1:],"t:b:p:sl:a:",["type=", "block=", "port=", "sys", "lane_mode=", "addr"])
   except getopt.GetoptError:
      print 'Usage: mt2_regs.py -t type -b block -p port -s -l lane_mode -a addr'
      sys.exit(2)

   for opt, arg in opts:
      if opt == '-h':
         print 'Usage: mt2_regs.py -t type -b block -p port -s -l lane_mode -a addr'
         sys.exit()
      elif opt in ("-t", "--type"):
         if arg in ("axi", "tsc", "pmd", "sbus"):
             type = arg
             if (type == "pmd"):
                 pmd = 1
         else:
             print 'Usage: mt2_regs.py -t type -b block -p port -s -l lane_mode -a addr'
             sys.exit(2)
      elif opt in ("-b", "--block"):
         block = int(arg)
      elif opt in ("-p", "--port"):
         port = int(arg)
      elif opt in ("-s", "--sys"):
         sys_side = 1
      elif opt in ("-l", "--lane_mode"):
         lane_mode = int(arg)
      elif opt in ("-a", "--addr"):
         addr = int(arg, 16) & 0xffffffff
      else:
         print 'mt2_regs.py -t type -b block -p port -s -l lane_mode -a addr'
         sys.exit(2)

   if (port & ~0x7) :
        print "Invalid port number: %d" % port
        sys.exit(2)

#   print "type = %d block = %d port = %d sys = %d lane_mode = %d addr = 0x%08x" % (type, block, port, sys_side, lane_mode, addr)
   if (type == "sbus"):
       print "type = %s block = %d port = %d sys=%d addr = 0x%08x" % (type, block, port, sys_side, addr)
       sbus_reg(addr, block, sys_side, port)
   elif type in ("tsc", "pmd"):
       print "type = %s sys = %d port = %d lane_mode = %d addr = 0x%08x" % (type, sys_side, port, lane_mode, addr)
       totsc_reg(addr, sys_side, port, lane_mode,  pmd)
   else:
       print "type = %s addr = 0x%08x" % (type, addr)
       axi_reg(addr)


