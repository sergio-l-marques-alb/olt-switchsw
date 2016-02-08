Procedimento para construir a build do Fastpath para o OLT7_8CH:

1 - Actualizar os scripts rcS_ptin, startup-config, fastpath e outros em FASTPATH/(...)/equipment
2 - Actualizar o ficheiro FASTPATH/PTin_FASTPATH_ReleaseNotes.txt com as ultimas Release Notes
3 - sh update.sh => Copia os binarios e scripts para a nova build
4 - sh build_fastpath.sh <version> => Constroi a build
5 - sh save_src.sh <version> => Salva os source files numa tarball

Done :)
