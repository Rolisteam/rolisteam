#
# Regular cron jobs for the rolisteam package
#
0 4	* * *	root	[ -x /usr/bin/rolisteam_maintenance ] && /usr/bin/rolisteam_maintenance
