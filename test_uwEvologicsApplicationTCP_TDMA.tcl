# ----------------------------------------------------------------------------------
# This script depicts a very simple but complete stack in which one or two nodes send data
# to a common sink. The second node can be used by the first one as a relay to send data to the sink.
# The routes are configured by using UW/STATICROUTING.
# The application used to generate data is UW/CBR.
# ----------------------------------------------------------------------------------
# Stack
#             Node 1                         Node 2                        Sink
#   +--------------------------+   +--------------------------+   +-------------+------------+
#   |  7. UW/APPLICATION       |   |  7. UW/APPLICATION       |   |  7. UW/APP  | UW/APP     |
#   +--------------------------+   +--------------------------+   +-------------+------------+
#   |  6. UW/UDP               |   |  6. UW/UDP               |   |  6. UW/UDP               |
#   +--------------------------+   +--------------------------+   +--------------------------+
#   |  5. UW/STATICROUTING     |   |  5. UW/STATICROUTING     |   |  5. UW/STATICROUTING     |
#   +--------------------------+   +--------------------------+   +--------------------------+
#   |  4. UW/IP                |   |  4. UW/IP                |   |  4. UW/IP                |
#   +--------------------------+   +--------------------------+   +--------------------------+
#   |  3. UW/MLL               |   |  3. UW/MLL               |   |  3. UW/MLL               |
#   +--------------------------+   +--------------------------+   +--------------------------+
#   |  2. UW/TDMA              |   |  2. UW/TDMA              |   |  2. UW/TDMA              |
#   +--------------------------+   +--------------------------+   +--------------------------+
#   |  1. UwModem/EvoLogicsS2C |   |  1. UwModem/EvoLogicsS2C |   |  1. UwModem/EvoLogicsS2C |
#   +--------------------------+   +--------------------------+   +--------------------------+
#            |         |                    |         |                   |         |       
#   +----------------------------------------------------------------------------------------+
#   |                                     UnderwaterChannel                                  |
#   +----------------------------------------------------------------------------------------+

######################################
# Flags to enable or disable options #
######################################
set opt(bash_parameters)    0

#####################
# Library Loading   #
#####################
load libMiracle.so
load libMiracleBasicMovement.so
load libmphy.so
load libmmac.so
load libUwmStd.so
load libuwinterference.so
load libuwip.so
load libuwstaticrouting.so
load libuwmll.so
load libuwudp.so
load libuwapplication.so
load libUwmStdPhyBpskTracer.so
load libuwphy_clmsgs.so
load libuwstats_utilities.so
load libuwphysical.so
load libpackeruwapplication.so
load libuwaloha.so
load libuwal.so
load libpackeruwip.so
load libpackercommon.so
load libpackermac.so
load libpackeruwudp.so
load libuwconnector.so
load libuwmodem.so
load libuwevologicss2c.so
load libuwmmac_clmsgs.so
load libuwcbr.so
load libuwtdma.so

#############################
# NS-Miracle initialization #
#############################
# You always need the following two lines to use the NS-Miracle simulator
set ns [new Simulator]
$ns use-Miracle

#Declare the use of a Real Time Schedule (necessary for the interfacing with real hardware)
$ns use-scheduler RealTime

##################
# Tcl variables  #
##################
set opt(nn)                 1
set opt(starttime)          5

set opt(stoptime)           400

set opt(txduration)         [expr $opt(stoptime) - $opt(starttime)]

set opt(rngstream)          1
set opt(pktsize)            64
set opt(cbr_period)         0.5
set opt(socket_comm)        1

if {$opt(bash_parameters)} {
    if {$argc != 4} {
        puts "ERROR!!!!!!"
	puts "The script requires four inputs:"
        puts "---> The DATA packet size (byte)"
        puts "---> Period to generate DATA packets (s)"
        puts "---> socket_comm"
        puts "---> Random generator substream"
        puts "For example: ns test_uwApplication.tcl 125 60 0 13"
        puts "Please try again."
        return
    } else {
        if {[lindex $argv 0] > 4096} {
		puts "ERROR!!!!!!"
		puts "The payload size is too high. It must be less then 4096 charachters."
		puts "Please try again."
		return
	} else {
		set opt(pktsize)       [lindex $argv 0]
		set opt(cbr_period)    [lindex $argv 1]
		set opt(socket_comm)   [lindex $argv 2]
		set opt(rngstream)     [lindex $argv 3]
	}
    }
} 

global defaultRNG
for {set k 0} {$k < $opt(rngstream)} {incr k} {
    $defaultRNG next-substream
}

set opt(tracefilename) "/dev/null"
set opt(tracefile) [open $opt(tracefilename) w]
set opt(cltracefilename) "/dev/null"
set opt(cltracefile) [open $opt(cltracefilename) w]

##################
# Tcl variables  #
##################
# address and port of the EvoLogics modem
set address_node(0) "10.42.9.1:9200"
set address_sink    "10.42.9.2:9200"

#########################
# Module Configuration  #
#########################

# variables for the TDMA module
Module/UW/TDMA set frame_duration   1
Module/UW/TDMA set debug_           10
Module/UW/TDMA set sea_trial_       1
Module/UW/TDMA set fair_mode        1 
Module/UW/TDMA set guard_time       0.3
Module/UW/TDMA set tot_slots        [expr $opt(nn) + 1]
Module/UW/TDMA set queue_size_      100

# variables for the AL module
Module/UW/AL set Dbit 1
Module/UW/AL set PSDU 64
Module/UW/AL set debug_ 1
Module/UW/AL set interframe_period  0.e1
Module/UW/AL set frame_set_validity 0

# variables for the packer(s)
UW/AL/Packer set SRC_ID_Bits 8
UW/AL/Packer set PKT_ID_Bits 8
UW/AL/Packer set FRAME_OFFSET_Bits 15
UW/AL/Packer set M_BIT_Bits 1
UW/AL/Packer set DUMMY_CONTENT_Bits 0
UW/AL/Packer set debug_ 1

NS2/COMMON/Packer set PTYPE_Bits 8
NS2/COMMON/Packer set SIZE_Bits 8
NS2/COMMON/Packer set UID_Bits 8
NS2/COMMON/Packer set ERROR_Bits 0
NS2/COMMON/Packer set TIMESTAMP_Bits 8
NS2/COMMON/Packer set PREV_HOP_Bits 8
NS2/COMMON/Packer set NEXT_HOP_Bits 8
NS2/COMMON/Packer set ADRR_TYPE_Bits 0
NS2/COMMON/Packer set LAST_HOP_Bits 0
NS2/COMMON/Packer set TXTIME_Bits 0
NS2/COMMON/Packer set debug_ 0

UW/IP/Packer set SAddr_Bits 8
UW/IP/Packer set DAddr_Bits 8
UW/IP/Packer set debug_ 0

NS2/MAC/Packer set Ftype_Bits 0
NS2/MAC/Packer set SRC_Bits 8
NS2/MAC/Packer set DST_Bits 8
NS2/MAC/Packer set Htype_Bits 0
NS2/MAC/Packer set TXtime_Bits 0
NS2/MAC/Packer set SStime_Bits 0
NS2/MAC/Packer set Padding_Bits 0
NS2/MAC/Packer set debug_ 1

UW/UDP/Packer set SPort_Bits 2
UW/UDP/Packer set DPort_Bits 2
UW/UDP/Packer set debug_ 0

UW/APP/uwApplication/Packer set SN_FIELD_ 6
UW/APP/uwApplication/Packer set RFFT_FIELD_ 5
UW/APP/uwApplication/Packer set RFFTVALID_FIELD_ 2
UW/APP/uwApplication/Packer set PRIORITY_FIELD_ 2
UW/APP/uwApplication/Packer set PAYLOADMSG_FIELD_SIZE_ 8
UW/APP/uwApplication/Packer set debug_ 10

Module/UW/APPLICATION set debug_ -1
Module/UW/APPLICATION set period_ $opt(cbr_period)
Module/UW/APPLICATION set PoissonTraffic_ 0
Module/UW/APPLICATION set Payload_size_ $opt(pktsize)
Module/UW/APPLICATION set drop_out_of_order_ 1
Module/UW/APPLICATION set EXP_ID_ 1

# variables for the S2C modem's interface
Module/UW/UwModem/EvoLogicsS2C set debug_         1
Module/UW/UwModem/EvoLogicsS2C set buffer_size    4096
Module/UW/UwModem/EvoLogicsS2C set max_read_size  4096
Module/UW/UwModem/EvoLogicsS2C set max_n_tx_poll  50
Module/UW/UwModem/EvoLogicsS2C set period_        0.1

################################
# Procedure(s) to create nodes #
################################
proc createNode { id } {

    global propagation data_mask ns app position node udp portnum ipr ipif uwal address_node
    global phy posdb opt rvposx mll mac db_manager
    global node_coordinates
    
    set node($id) [$ns create-M_Node $opt(tracefile) $opt(cltracefile)] 

    set app($id)  [new Module/UW/APPLICATION] 
    set udp($id)  [new Module/UW/UDP]
    set ipr($id)  [new Module/UW/StaticRouting]
    set ipif($id) [new Module/UW/IP]
    set mll($id)  [new Module/UW/MLL] 
    set mac($id)  [new Module/UW/TDMA] 
    set uwal($id) [new Module/UW/AL]
    set phy($id)  [new Module/UW/UwModem/EvoLogicsS2C]

    $node($id) addModule 8 $app($id)   1  "CBR"
    $node($id) addModule 7 $udp($id)   1  "UDP"
    $node($id) addModule 6 $ipr($id)   1  "IPR"
    $node($id) addModule 5 $ipif($id)  1  "IPF"   
    $node($id) addModule 4 $mll($id)   1  "MLL"
    $node($id) addModule 3 $mac($id)   1  "MAC"
    $node($id) addModule 2 $uwal($id)  1  "UWAL"
    $node($id) addModule 1 $phy($id)   1  "PHY"

    $node($id) setConnection $app($id)   $udp($id)   trace
    $node($id) setConnection $udp($id)   $ipr($id)   trace
    $node($id) setConnection $ipr($id)   $ipif($id)  trace
    $node($id) setConnection $ipif($id)  $mll($id)   trace
    $node($id) setConnection $mll($id)   $mac($id)   trace
    $node($id) setConnection $mac($id)   $uwal($id)  trace
    $node($id) setConnection $uwal($id)  $phy($id)   trace

    set portnum($id) [$udp($id) assignPort $app($id) ]
    if {$id > 254} {
      puts "hostnum > 254!!! exiting"
      exit
    }
    set general_adderss_ [expr ($id) + 1]
    $ipif($id) addr $general_adderss_
    $mac($id) setMacAddr $general_adderss_
    $mac($id) setSlotNumber [expr ($id) + 1]
    $phy($id) set ID_ $general_adderss_
    $phy($id) setModemAddress $address_node($id)
    $phy($id) setLogLevel DBG
    #$phy($id) setBurstMode
    
    # set packer for Adaptation Layer
    set packer($id) [new UW/AL/Packer]

    set packer_payload0 [new NS2/COMMON/Packer]
    set packer_payload1 [new UW/IP/Packer]
    set packer_payload2 [new NS2/MAC/Packer]
    set packer_payload3 [new UW/UDP/Packer]
    set packer_payload4 [new UW/APP/uwApplication/Packer]

    $packer_payload4 printMap

    $packer($id) addPacker $packer_payload0
    $packer($id) addPacker $packer_payload1
    $packer($id) addPacker $packer_payload2
    $packer($id) addPacker $packer_payload3
    $packer($id) addPacker $packer_payload4

    set prt_ [expr ($id) + 4000]
    $app($id) set Socket_Port_  $prt_
    $app($id) setSocketProtocol "TCP"
    $app($id) set node_ID_  $general_adderss_

    $uwal($id) linkPacker $packer($id)
    $uwal($id) set nodeID $general_adderss_
}

proc createSink { } {

    global propagation smask data_mask ns app_sink position_sink node_sink udp_sink portnum_sink interf_data_sink
    global phy_data_sink posdb_sink opt mll_sink mac_sink ipr_sink ipif_sink bpsk interf_sink uwal_sink address_sink

    set node_sink [$ns create-M_Node $opt(tracefile) $opt(cltracefile)]

    for {set cnt 0} {$cnt < $opt(nn)} {incr cnt} {
        set app_sink($cnt)  [new Module/UW/APPLICATION] 
    }
    set udp_sink       [new Module/UW/UDP]
    set ipr_sink       [new Module/UW/StaticRouting]
    set ipif_sink      [new Module/UW/IP]
    set mll_sink       [new Module/UW/MLL] 
    set mac_sink       [new Module/UW/TDMA]
    set uwal_sink      [new Module/UW/AL]
    set phy_data_sink  [new Module/UW/UwModem/EvoLogicsS2C]

    for { set cnt 0} {$cnt < $opt(nn)} {incr cnt} {
        $node_sink addModule 8 $app_sink($cnt) 1 "CBR"
    }
    $node_sink addModule 7 $udp_sink       1 "UDP"
    $node_sink addModule 6 $ipr_sink       1 "IPR"
    $node_sink addModule 5 $ipif_sink      1 "IPF"   
    $node_sink addModule 4 $mll_sink       1 "MLL"
    $node_sink addModule 3 $mac_sink       1 "MAC"
    $node_sink addModule 2 $uwal_sink      1 "UWAL"
    $node_sink addModule 1 $phy_data_sink  1 "PHY"

    for { set cnt 0} {$cnt < $opt(nn)} {incr cnt} {
        $node_sink setConnection $app_sink($cnt)  $udp_sink      trace   
    }
    $node_sink setConnection $udp_sink  $ipr_sink            trace
    $node_sink setConnection $ipr_sink  $ipif_sink           trace
    $node_sink setConnection $ipif_sink $mll_sink            trace 
    $node_sink setConnection $mll_sink  $mac_sink            trace
    $node_sink setConnection $mac_sink  $uwal_sink           trace
    $node_sink setConnection $uwal_sink $phy_data_sink       trace

    for { set cnt 0} {$cnt < $opt(nn)} {incr cnt} {
        set portnum_sink($cnt) [$udp_sink assignPort $app_sink($cnt)]
        if {$cnt > 252} {
            puts "hostnum > 252!!! exiting"
            exit
        }    
    }
    
    set general_adderss_ 32
    
    $ipif_sink addr $general_adderss_
    $mac_sink setMacAddr $general_adderss_
    $mac_sink setSlotNumber 0
    $phy_data_sink set ID_ $general_adderss_
    $phy_data_sink setModemAddress $address_sink
    $phy_data_sink setLogLevel DBG
    #$phy_data_sink setBurstMode

    # set packer for Adaptation Layer
    set packer_ [new UW/AL/Packer]

    set packer_payload0 [new NS2/COMMON/Packer]
    set packer_payload1 [new UW/IP/Packer]
    set packer_payload2 [new NS2/MAC/Packer]
    set packer_payload3 [new UW/UDP/Packer]
    set packer_payload4 [new UW/APP/uwApplication/Packer]
    
    $packer_payload4 printMap

    $packer_ addPacker $packer_payload0
    $packer_ addPacker $packer_payload1
    $packer_ addPacker $packer_payload2
    $packer_ addPacker $packer_payload3
    $packer_ addPacker $packer_payload4

    for {set cnt 0} {$cnt < $opt(nn)} {incr cnt} {
        set prt_ [expr ($cnt) + 5000]
        $app_sink($cnt) set Socket_Port_  $prt_
        $app_sink($cnt) setSocketProtocol "TCP"
        $app_sink($cnt) set node_ID_ $general_adderss_
    }

    $uwal_sink linkPacker $packer_
    $uwal_sink set nodeID $general_adderss_
}

#################
# Node Creation #
#################
# Create here all the nodes you want to network together
for {set id 0} {$id < $opt(nn)} {incr id}  {
    createNode $id
}
createSink

################################
# Inter-node module connection #
################################
proc connectNodes {id1} {
    global ipif ipr portnum app app_sink ipif_sink portnum_sink ipr_sink

    $app($id1) set destAddr_ [$ipif_sink addr]
    $app($id1) set destPort_ $portnum_sink($id1)
    $app_sink($id1) set destAddr_ [$ipif($id1) addr]
    $app_sink($id1) set destPort_ $portnum($id1)
}

# Setup flows
for {set id1 0} {$id1 < $opt(nn)} {incr id1}  {
    connectNodes $id1
}

# Fill ARP tables
for {set id1 0} {$id1 < $opt(nn)} {incr id1}  {
    for {set id2 0} {$id2 < $opt(nn)} {incr id2}  {
      $mll($id1) addentry [$ipif($id2) addr] [$mac($id2) addr]
    }   
    $mll($id1) addentry [$ipif_sink addr] [ $mac_sink addr]
    $mll_sink addentry [$ipif($id1) addr] [ $mac($id1) addr]
}

# Setup routing table
for {set id1 0} {$id1 < [expr $opt(nn) - 1]} {incr id1}  {
    set id2 [expr $id1 + 1]
    $ipr($id1) addRoute [$ipif_sink addr] [$ipif($id2) addr]
    $ipr_sink addRoute [$ipif($id1) addr] [$ipif($id1) addr]
}
set last_id [expr int($opt(nn) - 1)]
$ipr($last_id) addRoute [$ipif_sink addr] [$ipif_sink addr]
$ipr_sink addRoute [$ipif($last_id) addr] [$ipif($last_id) addr]


###################
# Final Procedure #
###################
# Define here the procedure to call at the end of the simulation
proc finish {} {
    global ns opt
    global mac propagation app_sink mac_sink phy_data phy_data_sink channel db_manager propagation
    global node_coordinates
    global ipr_sink ipr ipif udp app phy phy_data_sink
    global node_stats tmp_node_stats sink_stats tmp_sink_stats

    puts "---------------------------------------------------------------------"
    puts "Simulation summary"
    puts "number of nodes  : $opt(nn)"
    puts "packet size      : $opt(pktsize) byte"
    puts "app period       : $opt(cbr_period) s"
    puts "number of nodes  : $opt(nn)"
    puts "simulation length: $opt(txduration) s"
    puts "tx frequency     : $opt(freq) Hz"
    puts "tx bandwidth     : $opt(bw) Hz"
    puts "bitrate          : $opt(bitrate) bps"
    puts "---------------------------------------------------------------------"

    set sum_cbr_throughput     0
    set sum_per                0
    set sum_cbr_sent_pkts      0.0
    set sum_cbr_rcv_pkts       0.0    
    set sum_cbr_queue_pkts     0.0

    for {set i 0} {$i < $opt(nn)} {incr i}  {
        set cbr_throughput [$app_sink($i) getthr]
        set cbr_sent_pkts [$app($i) getsentpkts]
        
        set cbr_rcv_pkts [$app_sink($i) getrecvpkts]
        if { $opt(socket_comm) == 1 } {	
            set cbr_queue_pkts [$app($i) getrecvpktsqueue]
        }
        set sum_cbr_throughput [expr $sum_cbr_throughput + $cbr_throughput]
        set sum_cbr_sent_pkts  [expr $sum_cbr_sent_pkts + $cbr_sent_pkts]
        set sum_cbr_rcv_pkts   [expr $sum_cbr_rcv_pkts + $cbr_rcv_pkts]
        if { $opt(socket_comm) == 1 } {
            set sum_cbr_queue_pkts [expr $sum_cbr_queue_pkts + $cbr_queue_pkts]
        }
    }
        
    puts "Mean Throughput          : [expr ($sum_cbr_throughput/($opt(nn)))]"

    puts "Sent Packets             : $sum_cbr_sent_pkts"
    puts "Received Packets         : $sum_cbr_rcv_pkts"
    puts "Packet Delivery Ratio    : [expr $sum_cbr_rcv_pkts / $sum_cbr_sent_pkts * 100]"
  
    $ns flush-trace
    close $opt(tracefile)
}

###################
# start simulation
###################
for {set id1 0} {$id1 < $opt(nn)} {incr id1}  {
    $ns at 0                  "$phy($id1) start"
    $ns at $opt(starttime)    "$mac($id1) start"
    $ns at $opt(starttime)    "$app($id1) start"
    $ns at $opt(stoptime)     "$app($id1) stop"
    $ns at $opt(stoptime)     "$mac($id1) stop"
    $ns at $opt(stoptime)     "$phy($id1) stop"
    $ns at $opt(starttime)    "$app_sink($id1) start"
    $ns at $opt(stoptime)     "$app_sink($id1) stop"
}

$ns at 0                  "$phy_data_sink start"
$ns at $opt(starttime)    "$mac_sink start"
$ns at $opt(stoptime)     "$mac_sink stop"
$ns at $opt(stoptime)     "$phy_data_sink stop"

$ns at [expr $opt(stoptime) + 30.0] "finish; $ns halt"
$ns run
