#!/bin/bash

# as per http://superuser.com/questions/544436/rsync-between-two-local-directories
#rsync -a src_rsync/ src/
rsync -avh src/ src_rsync/
