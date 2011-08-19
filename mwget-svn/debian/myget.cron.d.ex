#
# Regular cron jobs for the myget package
#
0 4	* * *	root	[ -x /usr/bin/myget_maintenance ] && /usr/bin/myget_maintenance
