To create a new OLT version:

1. Compile all the necessary binaries inside fastpath folder:

   @fastpath$ make all

2. Copy the generated binaries into builds/apps folder at fastpath folder:

   @fastpath$ make install

3. At builds folder, update fp.releasenotes file with the latest updates:

   @builds$ vim fp.releasenotes

4. Run fastpath-olt.build script to create a generic tarball with all binaries:

   @builds$ fastpath-olt.build <version>
   ... or else use specific system builds:
   @builds$ fastpath-olt1t3.build <version>
   or
   @builds$ fastpath-olt360.build <version>

At the end of script execution, the image tarballs will be automatically
transferred to final location 10.112.15.243:/home/olt/fastpath_builds.
After binaries tarball transfer, you can proceed to next step.

5. After transfer, go to 10.112.15.243:/home/olt/fastpath_builds, and update the
   appropriate mkboards:

   @fastpath_builds$ sudo sh image_update.sh
   fastpath/fastpath-olt.image-v<version>-r<revision>.tgz /home/olt/<mkboards>/

And that's it. Good luck for the tests ;-)
