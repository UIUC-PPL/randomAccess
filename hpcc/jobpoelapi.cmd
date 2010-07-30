# System settings
#@ node_usage = not_shared
#@ network.LAPI = csss,,US
#@ node = 2
#@ tasks_per_node = 2

# User settings
#@ environment = COPY_ALL
#@ class = long

# Default Settings
#@ job_type = parallel
#@ wall_clock_limit = unlimited,unlimited
#@ notification = never
#@ queue

# System defined exports
MP_MSG_API=lapi
LAPI_USE_SHM=yes
MP_INTRDELAY=100
MP_EAGER_LIMIT=65536
MP_SHARED_MEMORY=yes
MP_USE_BULK_XFER=yes
MEMORY_AFFINITY=MCM
MP_TASK_AFFINITY=MCM
MP_EUILIB=us
