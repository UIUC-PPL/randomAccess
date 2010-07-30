#@ job_type = parallel
#@ environment = COPY_ALL
#@ job_name = mpiomp
#@ class = medium
#@ tasks_per_node = 1
#@ node = 4
#@ network.MPI_LAPI = sn_all,not_shared,US
#@ wall_clock_limit = 0:55:00
#@ queue

poe ./ra_mpiomp

