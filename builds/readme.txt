This folder has as purpose to save all the fastpath binaries,
and mark a personal version into them as identification.
Finally they will be transferred to where the final setup will be built.

To create a new OLT version:

(not necessary: step 3 executes this step)
1. Compile all the necessary binaries inside fastpath folder
   typing 'make all', or equivalent:

   @fastpath$ make all

(not necessary: step 3 executes this step)
2. Copy the generated binaries into builds/apps folder
   typing 'make install' or equivalent at fastpath folder:

   @fastpath$ make install

3. Run fastpath-olt.build script to create a generic tarball with all binaries,

   @builds$ fastpath-olt.build <version>

   ... or else use specific system builds:
   @builds$ fastpath-olt1t3.build <version>
   or
   @builds$ fastpath-olt360.build <version>

At the end of script execution, the image tarball will be automatically
transferred to final location 10.112.15.243:/home/olt/fastpath_builds.
In order for transfers to be successfull, you should have write permissions
at 10.112.42.243:/home/olt folder (for example being a gpon_builder).

Tarballs transfer are split into two stages, to minimize time availability:
 - The first will transfer the tarball with the binaries to update mkboards;
 - The second will transfer the unstripped binaries for debugging purposes.
After conclusion of the first stage, you can proceed to step 5.

4. After transfer, go to 10.112.15.243:/home/olt/fastpath_builds, and update the
   appropriate mkboards. This will unpach the binaries tarball into the given
   mkboards folder:

   @fastpath_builds$ sudo sh image_update.sh
   fastpath/fastpath-olt.image-v<version>-r<revision>.tgz /home/olt/<mkboards>/

5. Finally, go to mkboards of CXO160G and CXOLT1T0 and run fastpath-xxx.sh script to
   prepare the final tarball to be present inside 'files' folder.

Now, mkboards is ready to be used to build the setup file!

Good luck for the tests ;-)
