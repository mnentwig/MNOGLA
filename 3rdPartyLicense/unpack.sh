rm -Rf Noto_Sans
unzip -d Noto_Sans Noto_Sans.zip

rm -Rf freetype-VER-2-13-0
bzip2 -dc freetype-VER-2-13-0.tar.bz2 | tar -xvf -
# see INSTALL.ANY for each module's required .c files
sed -e '/t1_driver_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/cff_driver_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/t1cid_driver_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/pfr_driver_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/t42_driver_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/winfnt_driver_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/pcf_driver_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/bdf_driver_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/psaux_module_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/psnames_module_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/pshinter_module_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
# sed -e '/ft_smooth_renderer_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/ft_sdf_renderer_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
# sed -e '/ft_bitmap_sdf_renderer_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h
sed -e '/ft_svg_renderer_class/ s/^/\/\//' -i freetype-VER-2-13-0/include/freetype/config/ftmodule.h

rm -Rf glm-master
unzip glm-0.9.9.9.zip