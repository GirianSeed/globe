[SCE CONFIDENTIAL DOCUMENT]
PlayStation 2 Programmer Tool Runtime Library Release 3.0
      Copyright (C) 2000 Sony Computer Entertainment Inc.
                                      All Rights Reserved


[Sample Network Configuration Files]

The following sample network configuration files are included under /usr/local/sce/conf/net.

net000.cnf - Combination of ifc000.cnf and dev000.cnf
net001.cnf - Combination of ifc001.cnf and dev001.cnf
net002.cnf - Combination of ifc002.cnf and dev002.cnf
net003.cnf - Combination of ifc003.cnf and dev003.cnf
net004.cnf - Combination of ifc004.cnf and dev003.cnf
net005.cnf - Combination of ifc005.cnf and dev002.cnf
net006.cnf - Combination of ifc004.cnf and dev002.cnf


ifc000.cnf - type eth sample
ifc001.cnf - type ppp sample
ifc002.cnf - type nic sample
ifc003.cnf - type eth sample (use dhcp)
ifc004.cnf - pppoe sample
ifc005.cnf - type nic sample (use dhcp)

dev000.cnf - type eth sample
dev001.cnf - type ppp sample
dev002.cnf - type nic sample
dev003.cnf - type eth sample

* The ???000.cnf ~ ???001.cnf files can be used after changing the vendor and product in the dev files to appropriate identifiers, and the IP Address, User ID, Password etc. in the ifc files to appropriate values. 

* ???002.cnf can be used by changing the IP address, etc. in the ifc files to appropriate values.

* ???003.cnf and ???005.cnf can be used as is with sample programs.

* ???004.cnf and ???006.cnf can be used by changing the User ID, Password, etc. in the ifc files to appropriate values.


<net.db>
In the net.db configuration management file, data which can be obtained from the sceNetCnfList structure is arranged using ','.
(However, for type and stat the data is converted between binary and ASCII)
For the sceNetCnfList structure, refer to the "Common Network Setting Library" in the IOP Library Reference.  
For the usage of the configuration management file (adding an entry, etc.), refer to sce/iop/sample/inet/setapp. 

