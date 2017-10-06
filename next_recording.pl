#!/usr/bin/perl

# Shows the number of minutes until the next MythTV recording
# Light always on for the same period of time, ~1/4 second
# > 2 hours : always off
# 1 - 2 hours : sleep 2 seconds
# 30 - 60 minutes : sleep 1 second
# 15 + minute : sleep 1/2 second
# < 15 minutes : sleep 1/4 second
# recording : always on

# Get the results from the backend
$result = `mythtv-status -h amputechture --noscheduled-recordings --nostatus --no-encoders --nostatus`;

# Return 0 if there is currently a recording
if ($result =~ m/Recording now/i) {
    print "0\n";
    exit;
}

foreach $line (split /\n/, $result) {
    if ($line =~ m/Next Recording in/i) {
        print "$line\n";
        exit;
    }
}

# By default, say the next recording is happening 4 hours from now
print "240\n";
exit;

