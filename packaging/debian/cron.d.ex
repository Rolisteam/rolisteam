#
# Regular cron jobs for the rolisteam package
#
0 4	* * *	root	[ -x /usr/bin/rcse_maintenance ] && /usr/bin/rcse_maintenance
