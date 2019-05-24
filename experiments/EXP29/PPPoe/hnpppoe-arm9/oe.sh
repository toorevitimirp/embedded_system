#!/bin/sh
pppd pty "pppoe -I eth0 -T 3000" noipdefault noauth default-asyncmap defaultroute hide-password nodetach local mtu 1454 mru 1454 noaccomp noccp nopcomp novj novjccomp user 5325173
