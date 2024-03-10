# lemonbot

Bot used for merging Citra bleeding-edge builds.
At the moment, it uses the GitHub API to find builds to merge.

Pull requests to fix issues with the bot are more than welcome.

## Use in Citra bleeding-edge build process

The actual bot is ran on a server administrated by Flame Sage.
It is ran on a daily cron-job and updates from this repo before trying to merge builds.

## What does it do?

The bot will clone the latest `MAIN_REPO` and resync `PUSH_REPO`/master.
After that, each of the `PULL_REPOS` is visited.
It will look for PRs on GitHub labelled with `LABEL_TO_FETCH` and merge those onto the `MAIN_REPO`/master branch.
If a PR fails to merge it will be ignored.
Once everything has been merged the bot will push the resulting files to the `PUSH_REPO`/`self.branch_name` branch.

The path `self.tracking_path` is used as a working directory for the git merge operations.

The status will be logged via the `logger` object.
The default configuration is to log to *debug.log*, *error.log* and stdout.

## Installation

Download *tag_build.py* and configure lemonbot by modifying the configuration at the top of the file.
You can then run the script.

## License

lemonbot is licensed under the GPLv3 (or any later version). Refer to the license.txt file included.
