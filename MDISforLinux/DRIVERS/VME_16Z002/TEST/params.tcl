#
# Parameters used in tests
#

# Network parameters
set G_a11Ip     "192.1.1.29" 
set G_dutIp     "192.1.1.158" ;# device under test
set G_dut       "A14"  ;# A12,A13,A14,A15
set G_pldz002Rev    25 ;# decimal!

# other (fixed) parameters 
# Attention! no space between (linux)prompt and brackets!
set G_a11Prompt {KP/PPC: }
set G_linPrompt {sh\-3.00#}


# Login type to device under test
# 0 if telnet login
# 1 if ssh login
set G_haveSsh 0
set G_sshPasswd "menmen"



## vme4l_mtest -f -t=bwW -n=2 -s=4 a00000 a20000 -q=10
## Expect setup
##

# note: spawn must be called in a context above send/expect
#spawn -open $tty

match_max 20000


set G_a11WinsSetup 0
set G_a11Dmod(a24) -1
set G_a11Dmod(a32) -1

#
# 
# A11 routines 
# 
#

#
# Login to tsmon
#
proc a11-tsmon-login {} {
	global G_a11Prompt 

	upvar spawn_id spawn_id

	expect {
		"User name?: " {} 
		default { return "no A11 login" }
	}
	exp_send "kp2\r"

	expect {
		$G_a11Prompt {} 
		default { return "no A11 prompt" }
	}

}

#
# Fills A11 memory with bytes in fill
#
proc a11-fill-mem { addr fill } {
	global G_a11Prompt G_a11Ip
	set hwBugPrompt "HwBug> "

	spawn telnet $G_a11Ip
	set rv [a11-tsmon-login]
	if { $rv != "" } { return $rv }

	
	exp_send "hwbug\r"
	expect {
		$hwBugPrompt {} 
		default { return "no A11 hwbug prompt" }
	}

	exp_send "c $addr\r"
	
	foreach byte $fill {
		expect {
			-- {-> } {} 
			default { return "no A11 hwbug change prompt" }
		}
		exp_send "$byte\r"
	}
	expect {
		-- {->} {} 
		default { return "no A11 hwbug change prompt" }
	}
	exp_send "q\r"
	expect {
		$hwBugPrompt {} 
		default { return "no A11 hwbug prompt" }
	}
	exp_send "q\r"
	
	expect {
		$G_a11Prompt {} 
		default { return "no A11 prompt" }
	}

}

proc a11-spc-to-vmebltargs { spc } {

	switch $spc  {
	    11 { set arg "a16,d16,user,data" }
	    12 { set arg "a24,d16,user,data" }
	    13 { set arg "a32,d32,user,data" }
	    14 { set arg "a24,d16,user,data" }
	    15 { set arg "a32,d32,user,data" }
	}
	return $arg
}

proc a11-slvspc-to-cpuaddr { spc } {
	switch $spc  {
	    11 { set a11Addr 8cff0000 }
	    12 { set a11Addr 8c000000 }
	    13 { set a11Addr c1000000 }
	    14 { set a11Addr 8c000000 }
	    15 { set a11Addr c1000000 }
	}
	return $a11Addr
}

#
# Test if specified address responds
#
proc a11-vme-berr-check { spc vmeAddr } {
	global G_a11Prompt G_a11Ip

	spawn telnet $G_a11Ip
	set rv [a11-tsmon-login]
	if { $rv != "" } { return $rv }
	
	set mode [a11-spc-to-vmebltargs $spc]
	set endAddr [format %x [expr 4 + 0x$vmeAddr ]]
	exp_send "mtest2 -t=V -q=1 -m=$mode $vmeAddr $endAddr >/nil\r"

	expect {
		$G_a11Prompt { 
			return 0 
		} 
		"A bus trap error occurred" { 
			expect $G_a11Prompt
			return 1
		}
	}
}

#
# Fills VME memory from A11 with mtest2
#
proc a11-vme-fill-mtest { spc addr size } {
	global G_a11Prompt G_a11Ip

	spawn telnet $G_a11Ip
	set rv [a11-tsmon-login]
	if { $rv != "" } { return $rv }

	# fill a32 spaces with BLT 
	switch $spc {
		13 -
		15 {
			set mode [a11-spc-to-vmebltargs $spc]
			set endAddr [format %x [expr 0x$size + 0x$addr ]]
			exp_send "mtest2 -t=v -q=1 -m=$mode,blt $addr $endAddr >/nil\r"
		}
		default {
			set addr [format %x [expr 0x$addr + 0x[a11-slvspc-to-cpuaddr $spc] ]]
			set endAddr [format %x [expr 0x$size + 0x$addr ]]
			
			exp_send "mtest2 -t=l -q=1 $addr $endAddr >/nil\r"
		}
	}
	expect {
		$G_a11Prompt {} 
		default { return "no A11 prompt" }
	}
}

#
# Dump up to 16 bytes from A11 mem
#
proc a11-dump-mem { addr len } {
	
	global G_a11Prompt G_a11Ip
	set hwBugPrompt "HwBug> "

	spawn telnet $G_a11Ip
	set rv [a11-tsmon-login]
	if { $rv != "" } { return $rv }

	exp_send "hwbug\r"
	expect {
		$hwBugPrompt {} 
		default { return "no A11 hwbug prompt" }
	}

	exp_send "db $addr [format %x $len]\r"
	expect {
	    -re {0000: (([0-9a-f][0-9a-f] )+)} {
		set gotBytes $expect_out(1,string)
		set gotBytes [lrange $gotBytes 0 [expr $len -1]]
	    }
	    default { return "no hwbug response" }
	}

	return $gotBytes
}


proc setup-a11 { } {
	global G_a11Prompt G_a11Ip G_a11WinsSetup


	proc wait-a11 {} {
		global G_a11Prompt 

		upvar spawn_id spawn_id

		expect {
			$G_a11Prompt {} 
			default { return "no A11 prompt(2)" }
		}
	}

	proc wait-a11-store-dmodaddr { space } {
		global G_a11Prompt G_a11Dmod

		upvar spawn_id spawn_id

		expect {
			-re {modData=0x([0-9a-f]+)} {
				puts "DMOD ADDR=$expect_out(1,string)"
				set G_a11Dmod($space) $expect_out(1,string)
			}
			default { return "no A11 dmod addr" }
		}

		expect {
			$G_a11Prompt {

			} 
			default { return "no A11 prompt" }
		}
	}

	if { $G_a11WinsSetup } {
		puts "A11 address windows already setup"
		return
	}

	spawn telnet $G_a11Ip
	set rv [a11-tsmon-login]
	if { $rv != "" } { return $rv }


	# remove slave data modules 
	exp_send "vme_slavedm -n=vmea24 -r\r"
	if { [set rv [wait-a11]] != {} } { return $rv }
	exp_send "vme_slavedm -n=vmea32 -r\r"
	if { [set rv [wait-a11]] != {} } { return $rv }

	# create slave data modules

	exp_send "vme_slavedm -n=vmea24 -m=a24,user,data,pren,pwen -s=20000 -a=a00000\r"
	if { [set rv [wait-a11-store-dmodaddr a24]] != {} } { return $rv }

	exp_send "vme_slavedm -n=vmea32 -m=a32,user,data,pren,pwen -s=100000 -a=80000000\r"
	#exp_send "vme_slavedm -n=vmea32 -m=a32,user,data -s=100000 -a=80000000\r"
	if { [set rv [wait-a11-store-dmodaddr a32]] != {} } { return $rv }

	set G_a11WinsSetup 1


	exp_send "logout\r"
}

#puts [setup-a11]
