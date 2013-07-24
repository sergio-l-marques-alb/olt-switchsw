This folder has as purpose to save all the fastpath binaries,
and mark a personal version into them as identification.
Finally they will be transferred to where the final setup will be built.

This folder should be located at the same level as the
'branches', 'tags' and 'trunk' folders.
After decompressing this folder, move it one level back:
$ mv builds ..

To create a new OLT version:

1. Update release notes in 'fastpath.releasenotes' file.

2. Update binary files to 'apps' folder using the make install_? command in related projects
   $ make install_olt1t3
   or
   $ make install_olt360

3. Run fastpath-olt.build script to create a generic tarball with all binaries,
   $ fastpath-olt.build <version>
   ... or else use specific system builds:
   $ fastpath-olt1t3.build <version>
   or
   $ fastpath-olt360.build <version>

At the end of scripts execution, the final tarball will be automatically transferred
to final location 10.112.15.243:/home/olt/fastpath_builds.
