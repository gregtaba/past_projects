#!/bin/bash
# Run commands ./jobCommander poll running and ./jobCommander poll queued to get a list of running and queued jobs
running_jobs=$(./jobCommander poll running)
queued_jobs=$(./jobCommander poll queued)
# using , as a delimiter we take job_xx from every tripleta in the running_jobs
running2_jobs=$(echo "$running_jobs" | awk -F',' '{print $1}')
# we go through every job_xx in running2_jobs and stop  it
while IFS= read -r job; do
    if [[ "$job" == job_* ]]; then
        ./jobCommander stop "$job"
    fi
done <<< "$running2_jobs"
# using , as a delimiter we take job_xx from every tripleta in the queued_jobs
queued2_jobs=$(echo "$queued_jobs" | awk -F',' '{print $1}')
# we go through every job_xx in queued2_jobs and stop  it
while IFS= read -r job; do
    if [[ "$job" == job_* ]]; then
        ./jobCommander stop "$job"
    fi
done <<< "$queued2_jobs"
