# MDIS4Linux_systemPackage
This is the main repository of the MEN Driver interface system (MDIS) package for Linux, article nr. 13MD05-90.
It contains several submodules, so be sure to not only clone this repository but also to update submodules. Below the complete cloning is shown as reference:

[tschnuer@tslinux3 Work]$ git clone git@github.com:ThomasSchnuerer/MDIS4Linux_systemPackage.git
Cloning into 'MDIS4Linux_systemPackage'...
remote: Counting objects: 2316, done.
remote: Compressing objects: 100% (54/54), done.
remote: Total 2316 (delta 21), reused 0 (delta 0), pack-reused 2256 Receiving objects: 100% (2316/2316), 21.75 MiB | 13.22 MiB/s, done.
Resolving deltas: 100% (1276/1276), done.
[tschnuer@tslinux3 Work]$ cd MDIS4Linux_systemPackage/
[tschnuer@tslinux3 MDIS4Linux_systemPackage]$
[tschnuer@tslinux3 MDIS4Linux_systemPackage]$
[tschnuer@tslinux3 MDIS4Linux_systemPackage]$ git submodule update --init --recursive Submodule 'DRIVERS/BBIS/A12' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_a12_com.git) registered for path 'DRIVERS/BBIS/A12'
Submodule 'DRIVERS/BBIS/A201' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_a201_com.git) registered for path 'DRIVERS/BBIS/A201'
Submodule 'DRIVERS/BBIS/A203N' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_a203n_com.git) registered for path 'DRIVERS/BBIS/A203N'
Submodule 'DRIVERS/BBIS/A21' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_a21_com.git) registered for path 'DRIVERS/BBIS/A21'
Submodule 'DRIVERS/BBIS/CHAMELEON' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_chameleon_com.git) registered for path 'DRIVERS/BBIS/CHAMELEON'
Submodule 'DRIVERS/BBIS/D201' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_d201_com.git) registered for path 'DRIVERS/BBIS/D201'
Submodule 'DRIVERS/BBIS/D203' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_d203n_com.git) registered for path 'DRIVERS/BBIS/D203'
Submodule 'DRIVERS/BBIS/D302' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_d302_com.git) registered for path 'DRIVERS/BBIS/D302'
Submodule 'DRIVERS/BBIS/ISA' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_isa_com.git) registered for path 'DRIVERS/BBIS/ISA'
Submodule 'DRIVERS/BBIS/PCI' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_pci_com.git) registered for path 'DRIVERS/BBIS/PCI'
Submodule 'DRIVERS/BBIS/SMBPCI' (git@github.com:ThomasSchnuerer/mdis_drivers_bbis_smbpci_com.git) registered for path 'DRIVERS/BBIS/SMBPCI'
Submodule 'INCLUDE/COM/MEN' (git@github.com:ThomasSchnuerer/mdis_include_com_men.git) registered for path 'INCLUDE/COM/MEN'
Submodule 'LIBSRC/CHAMELEON/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_chameleon_com.git) registered for path 'LIBSRC/CHAMELEON/COM'
Submodule 'LIBSRC/DESC/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_desc_com.git) registered for path 'LIBSRC/DESC/COM'
Submodule 'LIBSRC/ID/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_id_com.git) registered for path 'LIBSRC/ID/COM'
Submodule 'LIBSRC/MBUF/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_mbuf_com.git) registered for path 'LIBSRC/MBUF/COM'
Submodule 'LIBSRC/PLD/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_pld_com.git) registered for path 'LIBSRC/PLD/COM'
Submodule 'LIBSRC/SMB/PORT/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_smb_port_com.git) registered for path 'LIBSRC/SMB/PORT/COM'
Submodule 'LIBSRC/SMB/PORTCB/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_smb_portcb_com.git) registered for path 'LIBSRC/SMB/PORTCB/COM'
Submodule 'LIBSRC/SMB2/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_smb2_com.git) registered for path 'LIBSRC/SMB2/COM'
Submodule 'LIBSRC/USR_OSS' (git@github.com:ThomasSchnuerer/mdis_libsrc_usr_oss.git) registered for path 'LIBSRC/USR_OSS'
Submodule 'LIBSRC/USR_UTL/COM' (git@github.com:ThomasSchnuerer/mdis_libsrc_usr_utl_com.git) registered for path 'LIBSRC/USR_UTL/COM'
Submodule 'TOOLS/MDIS_API' (git@github.com:ThomasSchnuerer/mdis_tools_mdis_api.git) registered for path 'TOOLS/MDIS_API'
Submodule 'TOOLS/M_TMR' (git@github.com:ThomasSchnuerer/mdis_tools_m_tmr.git) registered for path 'TOOLS/M_TMR'
Submodule 'TOOLS/WDOG' (git@github.com:ThomasSchnuerer/mdis_tools_wdog.git) registered for path 'TOOLS/WDOG'
Cloning into 'DRIVERS/BBIS/A12'...
remote: Counting objects: 42, done.
remote: Compressing objects: 100% (25/25), done.
remote: Total 42 (delta 7), reused 42 (delta 7), pack-reused 0 Receiving objects: 100% (42/42), 24.22 KiB | 0 bytes/s, done.
Resolving deltas: 100% (7/7), done.
Submodule path 'DRIVERS/BBIS/A12': checked out '701722ba75225ecf6c1a43708400eaeacba1cdbe'
Cloning into 'DRIVERS/BBIS/A201'...
remote: Counting objects: 64, done.
remote: Compressing objects: 100% (24/24), done.
remote: Total 64 (delta 31), reused 64 (delta 31), pack-reused 0 Receiving objects: 100% (64/64), 33.93 KiB | 0 bytes/s, done.
Resolving deltas: 100% (31/31), done.
Submodule path 'DRIVERS/BBIS/A201': checked out '0b32a6d7b17cb47ae7ffb341b16750657f62ece1'
Cloning into 'DRIVERS/BBIS/A203N'...
remote: Counting objects: 121, done.
remote: Compressing objects: 100% (57/57), done.
remote: Total 121 (delta 42), reused 121 (delta 42), pack-reused 0 Receiving objects: 100% (121/121), 82.06 KiB | 0 bytes/s, done.
Resolving deltas: 100% (42/42), done.
Submodule path 'DRIVERS/BBIS/A203N': checked out '562f31b7f7e1d0e5243f645fe32a00a64e7e39e2'
Cloning into 'DRIVERS/BBIS/A21'...
remote: Counting objects: 47, done.
remote: Compressing objects: 100% (23/23), done.
remote: Total 47 (delta 12), reused 47 (delta 12), pack-reused 0 Receiving objects: 100% (47/47), 19.41 KiB | 0 bytes/s, done.
Resolving deltas: 100% (12/12), done.
Submodule path 'DRIVERS/BBIS/A21': checked out 'bd7f6d876e3b45036d5522b2e5663eabb70dc943'
Cloning into 'DRIVERS/BBIS/CHAMELEON'...
remote: Counting objects: 99, done.
remote: Compressing objects: 100% (40/40), done.
remote: Total 99 (delta 41), reused 99 (delta 41), pack-reused 0 Receiving objects: 100% (99/99), 80.33 KiB | 0 bytes/s, done.
Resolving deltas: 100% (41/41), done.
Submodule path 'DRIVERS/BBIS/CHAMELEON': checked out 'f849ae35c700bb31e3fb447d4f1297dbe06267b4'
Cloning into 'DRIVERS/BBIS/D201'...
remote: Counting objects: 99, done.
remote: Compressing objects: 100% (30/30), done.
remote: Total 99 (delta 59), reused 99 (delta 59), pack-reused 0 Receiving objects: 100% (99/99), 82.61 KiB | 0 bytes/s, done.
Resolving deltas: 100% (59/59), done.
Submodule path 'DRIVERS/BBIS/D201': checked out '8298f2fac3467f074bee60efc0fa8b4737af2f44'
Cloning into 'DRIVERS/BBIS/D203'...
remote: Counting objects: 65, done.
remote: Compressing objects: 100% (27/27), done.
remote: Total 65 (delta 26), reused 65 (delta 26), pack-reused 0 Receiving objects: 100% (65/65), 42.98 KiB | 0 bytes/s, done.
Resolving deltas: 100% (26/26), done.
Submodule path 'DRIVERS/BBIS/D203': checked out '7f40f943f52cce3201215f1f92e9f6ad183a1174'
Cloning into 'DRIVERS/BBIS/D302'...
remote: Counting objects: 48, done.
remote: Compressing objects: 100% (25/25), done.
remote: Total 48 (delta 12), reused 48 (delta 12), pack-reused 0 Receiving objects: 100% (48/48), 26.44 KiB | 0 bytes/s, done.
Resolving deltas: 100% (12/12), done.
Submodule path 'DRIVERS/BBIS/D302': checked out '090c01df1c6837682c5bc125cbeea3321ab54c3a'
Cloning into 'DRIVERS/BBIS/ISA'...
remote: Counting objects: 42, done.
remote: Compressing objects: 100% (21/21), done.
remote: Total 42 (delta 11), reused 42 (delta 11), pack-reused 0 Receiving objects: 100% (42/42), 21.22 KiB | 0 bytes/s, done.
Resolving deltas: 100% (11/11), done.
Submodule path 'DRIVERS/BBIS/ISA': checked out 'f7e83dca9f77341404de04370633e4d037998ddc'
Cloning into 'DRIVERS/BBIS/PCI'...
remote: Counting objects: 80, done.
remote: Compressing objects: 100% (27/27), done.
remote: Total 80 (delta 40), reused 80 (delta 40), pack-reused 0 Receiving objects: 100% (80/80), 36.65 KiB | 0 bytes/s, done.
Resolving deltas: 100% (40/40), done.
Submodule path 'DRIVERS/BBIS/PCI': checked out 'd23c8b752f9b044095068de13e1e3a4bda36e7d5'
Cloning into 'DRIVERS/BBIS/SMBPCI'...
remote: Counting objects: 59, done.
remote: Compressing objects: 100% (22/22), done.
remote: Total 59 (delta 31), reused 59 (delta 31), pack-reused 0 Receiving objects: 100% (59/59), 27.62 KiB | 0 bytes/s, done.
Resolving deltas: 100% (31/31), done.
Submodule path 'DRIVERS/BBIS/SMBPCI': checked out 'b80b28e26716567d8b4141b277724b290ab7ee9b'
Cloning into 'INCLUDE/COM/MEN'...
remote: Counting objects: 198, done.
remote: Compressing objects: 100% (118/118), done.
remote: Total 198 (delta 78), reused 198 (delta 78), pack-reused 0 Receiving objects: 100% (198/198), 183.27 KiB | 0 bytes/s, done.
Resolving deltas: 100% (78/78), done.
Submodule path 'INCLUDE/COM/MEN': checked out '07218d79c4ec62154fd159ce7f4ad3859b18c631'
Cloning into 'LIBSRC/CHAMELEON/COM'...
remote: Counting objects: 124, done.
remote: Compressing objects: 100% (49/49), done.
remote: Total 124 (delta 71), reused 124 (delta 71), pack-reused 0 Receiving objects: 100% (124/124), 126.42 KiB | 0 bytes/s, done.
Resolving deltas: 100% (71/71), done.
Submodule path 'LIBSRC/CHAMELEON/COM': checked out 'ad9983376aa50f57d78b67365a2ebe0ffd4ffb06'
Cloning into 'LIBSRC/DESC/COM'...
remote: Counting objects: 41, done.
remote: Compressing objects: 100% (31/31), done.
remote: Total 41 (delta 7), reused 41 (delta 7), pack-reused 0 Receiving objects: 100% (41/41), 16.92 KiB | 0 bytes/s, done.
Resolving deltas: 100% (7/7), done.
Submodule path 'LIBSRC/DESC/COM': checked out '435670248aa43f5508bdeca0e323122fd5ba0311'
Cloning into 'LIBSRC/ID/COM'...
remote: Counting objects: 51, done.
remote: Compressing objects: 100% (30/30), done.
remote: Total 51 (delta 18), reused 51 (delta 18), pack-reused 0 Receiving objects: 100% (51/51), 41.80 KiB | 0 bytes/s, done.
Resolving deltas: 100% (18/18), done.
Submodule path 'LIBSRC/ID/COM': checked out 'c806193b00144bf57772bdacefb0c232aee89b41'
Cloning into 'LIBSRC/MBUF/COM'...
remote: Counting objects: 26, done.
remote: Compressing objects: 100% (17/17), done.
remote: Total 26 (delta 5), reused 26 (delta 5), pack-reused 0 Receiving objects: 100% (26/26), 19.34 KiB | 0 bytes/s, done.
Resolving deltas: 100% (5/5), done.
Submodule path 'LIBSRC/MBUF/COM': checked out '8874bf0cc06f4b455cf405bf3733ff7268aca2d4'
Cloning into 'LIBSRC/PLD/COM'...
remote: Counting objects: 33, done.
remote: Compressing objects: 100% (23/23), done.
remote: Total 33 (delta 6), reused 33 (delta 6), pack-reused 0 Receiving objects: 100% (33/33), 15.51 KiB | 0 bytes/s, done.
Resolving deltas: 100% (6/6), done.
Submodule path 'LIBSRC/PLD/COM': checked out '2b9ca6194bca0c311ff1f900a3512e6e7abe3455'
Cloning into 'LIBSRC/SMB/PORT/COM'...
remote: Counting objects: 10, done.
remote: Compressing objects: 100% (9/9), done.
remote: Total 10 (delta 1), reused 10 (delta 1), pack-reused 0 Receiving objects: 100% (10/10), 8.09 KiB | 0 bytes/s, done.
Resolving deltas: 100% (1/1), done.
Submodule path 'LIBSRC/SMB/PORT/COM': checked out 'f3ba89da4a12daa626886bac65b90412fbce22e5'
Cloning into 'LIBSRC/SMB/PORTCB/COM'...
remote: Counting objects: 3, done.
remote: Compressing objects: 100% (2/2), done.
remote: Total 3 (delta 0), reused 3 (delta 0), pack-reused 0 Receiving objects: 100% (3/3), 5.09 KiB | 0 bytes/s, done.
Submodule path 'LIBSRC/SMB/PORTCB/COM': checked out '5298fafc51d1c5f96302f4fcd8c61b0003febeb1'
Cloning into 'LIBSRC/SMB2/COM'...
remote: Counting objects: 128, done.
remote: Compressing objects: 100% (49/49), done.
remote: Total 128 (delta 73), reused 128 (delta 73), pack-reused 0 Receiving objects: 100% (128/128), 169.76 KiB | 0 bytes/s, done.
Resolving deltas: 100% (73/73), done.
Submodule path 'LIBSRC/SMB2/COM': checked out '5096c40fca0d127bb4b1899eb089b9e7475ba7c8'
Cloning into 'LIBSRC/USR_OSS'...
remote: Counting objects: 71, done.
remote: Compressing objects: 100% (39/39), done.
remote: Total 71 (delta 27), reused 71 (delta 27), pack-reused 0 Receiving objects: 100% (71/71), 62.71 KiB | 0 bytes/s, done.
Resolving deltas: 100% (27/27), done.
Submodule path 'LIBSRC/USR_OSS': checked out 'dae6575561a11249d366f7618f9301cc21578a5d'
Cloning into 'LIBSRC/USR_UTL/COM'...
remote: Counting objects: 61, done.
remote: Compressing objects: 100% (38/38), done.
remote: Total 61 (delta 20), reused 61 (delta 20), pack-reused 0 Receiving objects: 100% (61/61), 40.07 KiB | 0 bytes/s, done.
Resolving deltas: 100% (20/20), done.
Submodule path 'LIBSRC/USR_UTL/COM': checked out '20325c3ec005948e73bebdd276335edba09d96dc'
Cloning into 'TOOLS/MDIS_API'...
remote: Counting objects: 68, done.
remote: Compressing objects: 100% (25/25), done.
remote: Total 68 (delta 25), reused 68 (delta 25), pack-reused 0 Receiving objects: 100% (68/68), 16.63 KiB | 0 bytes/s, done.
Resolving deltas: 100% (25/25), done.
Submodule path 'TOOLS/MDIS_API': checked out '99576bec2a196d126acbcc8c63e82258187743de'
Cloning into 'TOOLS/M_TMR'...
remote: Counting objects: 15, done.
remote: Compressing objects: 100% (8/8), done.
remote: Total 15 (delta 2), reused 15 (delta 2), pack-reused 0 Receiving objects: 100% (15/15), 5.34 KiB | 0 bytes/s, done.
Resolving deltas: 100% (2/2), done.
Submodule path 'TOOLS/M_TMR': checked out '341de4691b10be33d89a9607d40f8e3ddf52effc'
Cloning into 'TOOLS/WDOG'...
remote: Counting objects: 59, done.
remote: Compressing objects: 100% (35/35), done.
remote: Total 59 (delta 10), reused 59 (delta 10), pack-reused 0 Receiving objects: 100% (59/59), 13.19 KiB | 0 bytes/s, done.
Resolving deltas: 100% (10/10), done.
Submodule path 'TOOLS/WDOG': checked out 'f885bb3be8111b42d816d0f6e760c34dea2ef2d4'
[tschnuer@tslinux3 MDIS4Linux_systemPackage]$

