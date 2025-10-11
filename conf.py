# This conf.py file describes how to build Software project Vestel Mstar SingleChipTV Platform mb181
import os
import string
import socket
import os.path
import subprocess
import platform

###############################################################################
extra_build_options = (
    cbuild_option(
        name='vbilog',
        description='Turn on vbilog',
        default=None),

    cbuild_option(
        name='freespace',
        description='Turn on freespace',
        default=None),

    cbuild_option(
        name='use_gles',
        description='use opengles v2 backend for Vewd',
        requires=['vewd_browser'],
        default=1),

    cbuild_option(
        name='clnx',
        description='Turn on option if toolchain is not located as in mstar colinux virtual machine',
        default=1),

    cbuild_option(
        name='environment_generator',
        description='Run environment generator script and made a delivery package',
        default=None),

    cbuild_option(
        name='mpeg4',
        description='Turn on mpeg4 video support',
        default=1),

    cbuild_option(
        name='cleanenv',
        description='Turn on option if a need to make clean environment',
        default=1),

    cbuild_option(
        name='nand512',
        description='turn on 512MB NAND flash support',
        default=None),

    cbuild_option(
        name='emmc',
        description='turn on 4GB EMMC support',
        default=None),

    cbuild_option(
        name='tee',
        description='turn on TEE support',
        default=1),

    cbuild_option(
        name='sec',
        description='turn on SEC support',
        default=None),

    cbuild_option(
        name='secboot',
        description='enable secure booting',
        default=None),

    cbuild_option(
        name='netflix',
        description='NetFlix support',
        default=None),

    cbuild_option(
        name='apm',
        description='apm support',
        default=None),

    cbuild_option(
        name='super_resolution',
        description='turn on super_resolution',
        default=None),

    cbuild_option(
        name='dynamic_lib',
        description='use mstar dynamic lib',
        default=None),

    cbuild_option(
        name='g10',
        description='compile for MStar G10',
        default=None),

    cbuild_option(
        name='g36',
        description='compile for MTK G36',
        default=None),

    cbuild_option(
        name='g32',
        description='compile for MTK G32',
        default=None),

    cbuild_option(
        name='widevine',
        description='widevine v3.0 support',
        default=None),

    cbuild_option(
        name='hard_float',
        description='use floating point unit (required hardware support)',
        default=1),

    cbuild_option(
        name='oprofile',
        description='releases oprofile tool',
        default=None),

    cbuild_option(
        name='valgrind',
        description='releases valgrind tool',
        default=None),

    cbuild_option(
        name='mmap_15G',
        description='Selects 1.5GByte memory configuration',
        default=None),

    cbuild_option(
        name='mmap_20G',
        description='Selects 2.0GByte memory configuration',
        default=None),
       
    cbuild_option(
        name='mmap_125G',
        description='Selects 1.25GByte memory configuration',
        default=None),

    cbuild_option(
        name='mmap_10G',
        description='Selects 1.0GByte memory configuration',
        default=None),

    cbuild_option(
        name='mmap_768M',
        description='Selects 768MByte memory configuration',
        default=None),

    cbuild_option(
        name='mmap_0p5G',
        description='Selects 0.5GByte memory configuration',
        default=None),

    cbuild_option(
        name='connected',
        description='connected module',
        default=None),

    cbuild_option(
        name='nghttp2',
        description='copy nghttp2 libs needed by libcurl with nghttp2 support',
        default=1),

    cbuild_option(
        name='dev_sdk',
        description='Select development sdk',
        default=None),

    cbuild_option(
        name='debug_dsr',
        description='Enable debug dsr',
        default=None),

    cbuild_option(
        name='telnetd',
        description='Enable telnetd',
        default=None),
    
    cbuild_option(
        name='uk2023_legacy_projects',
        description='Enable support for uk2023 legacy projects (Quigon/Obiwan)',
        default=None)
)
###############################################################################


###############################################################################
register_targets((
    ('bin',              'Build a compressed bin HEX image, suitable for serial download'),
    ('nfs',              'Create an nfsroot image ready to run from nfs.'),
    ('build_dynamic_ui', 'Build only dynamic ui files (use options=dynamic_ui --clean)'),
    ('nothing',          'Build nothing'),
    ('vewdlib',          'Build libraries for Vewd'),
    ('mstarlib',         'Build libraries for Mstar'),
    ('amazonlib',        'build libraries for Amazon'),
))
###############################################################################

cbuild_first = 'CBUILD_FIRST'
pre_order = ''
_sqlite_db_file_path = '"/conf/sqlite_db/settingsDB.db"'
_sqlite_db_backup_file_path = '"/conf/sqlite_db/settingsDBBackup.db"'
_demo_files_path = '"/data/private/vestel/Demo/"'
_internal_customer_media_path = '"/data/private/vestel/customer/"'
_wink_animation_audio_file_path = None
_ice_fs_cache_file_path = '"666,x:/conf/nvm_storage"'
_ice_fs_conf_path = '"/conf"'
_opapp_configuration_dir_path = '"/vendor/opapp"'
_opapp_working_dir_path = '"/data/public/browser/opapp"'
_downloaded_images_dir_path = '"/conf/downloaded_images"'
_gracenote_user_id_file = '"/conf/gracenote_user_id"'
_starturl_file_path = '"/conf/browser/starturl.txt"'
_bt_start_server_conf_path = '"/conf/"'
ice_source_directory = 'ice_product_mstar_g3x'
_dsmcc_memory_free_before_allocate = 1
_dsmcc_dcf_module_size_limit = (2*1024*1024)
_brcm = None
_mstar_platform = 1
_novatek_platform = None
_tuning_time_improvement = 1
libDFB = ''
libDFB_ld_flag = ''
discretix_ld_flag = ''
vmx_web_ld_flag = ''
config_directory = 'config_mb180/defaults'
certificate_path = '/conf/tvcertificate'
connected_str = ''
_use_zlib_for_ubifs = 0
nfsbuild = 'not_nfs'
_enable_nfsbuild = 'nfs' in options.targets
_mstar_chip = ''
_model_year = ''
_mmap_config = ''
directfb_dist = ''
_mstar_bsp = ''
discretix_release_dir = None
vmx_web_lib_release = None
vmx_web_lib_release_dir = None
widevine_release_dir = None
_storage = 'emmc'
_storage_size = 0
_use_websocket_support = 1
_include_oss_information = 1
_include_widi_support = 1
_video_position_delay = 1
_build_drm_backend = 0
_build_ciplus_drm_backend = 0
_include_widevine_support = 0
_include_verimatrix_support = 0
_use_dial_164 = None
_vestel_hdcp = None
_str_support = 1
_include_xpca = 0
_mstar_cusInfo = ''
_include_hw_thumbnail_decoding = 1
_include_extended_logging = 1
_include_hotel_tv = 0
_include_rf_hotel_tv_resources = '"/buffer/public/rfhoteltv/"'
_include_rf_hotel_tv_temp_resources = '"/tmp/rfhoteltv/"'
_include_rf_hotel_tv_clone_from_usb_path = '"/conf/sqlite_db/"'
_include_rf_hotel_tv_default_app = '"/vendor/html_applications/rfhoteltv/coaxialclient/Pages/Default.html"'
_include_rf_hotel_tv_app_url_file = '"/conf/browser/starturl.txt"'
_include_hotel_tv_channel_list_path = '"/conf/hoteltv/ip_tuner/channelTable.xml"'
_include_hotel_tv_channel_list_path_parent = '"/conf/hoteltv"'
_include_hotel_tv_temporary_channel_list_path = '"/tmp/channelTable.xml"'
_include_hotel_tv_buffer_access_path = '"/buffer/plugin"'
_include_hotel_tv_buffer_public_access_path = '"/buffer/public"'
_include_hotel_tv_tmp_access_path = '"/tmp"'
_include_hotel_tv_custom_html_ui_access_path = '"/buffer/public/custom_html_ui"'

_include_lighttpd_web_server = 1

EXTERNAL_DELIVERY_PLUGINS_DIR = 'libs_plugins'

def libname(lib):
    """Create the makefile function command to generate a static library name.
    
    Parameters:
        lib - The name to use to generate the static library name.

    Returns:
        The library name makefile command.
    
    """

    return '$(call libname,{})'.format(lib)

def subfolder(path):
    """Extract the last directory from a given path.
    
    Parameters:
        path - The full path from which to extract the last directory. 

    Returns:
        The last directory.
    
    """
    return os.path.basename(os.path.normpath(path))

############################################################################

# OAD identifiers
_oui = '0009df'      # manufacturers organisational unique identifier
_dcf_filename = '4951.dcf' # unique identifier for the product model within the manufacturers range
_full_dcf_filename = '/' + _oui + '/' + _dcf_filename

# CHIL identifier
_chil_receiver_ident = '180'

# Set the device model variable (we will determine it from the directory name this conf.py is in)
_device_model = 'mb180'

# Set WhoAmI identifier str as OBIWAN
_receiver_identifier_str = 'OBIWAN'

# for use script parameter
mb_name = 'mb180'

# for sw config tool
mb_name_sw_config_tool = mb_name

# Select Vewd SDK variants for SDKs used in this conf.py.
vewd_sdk_variants = ['LIMA2']   # LIMA2 is used for SDK423

############################################################################
_avs_board_type = 'mb181'
execfile ('../aurora.py')
############################################################################
project_root = os.path.abspath('../..')

if include_options.telnetd:
    _enable_telnetd = 1
else:
    _enable_telnetd = 0

_disable_dsr = ''  #MP : '', debug : '.debug'
if include_options.debug_dsr:
    _disable_dsr = '.debug'
elif 'D' in canonical_variant:
    _disable_dsr = '.debug'
else:
    set_production(True)

if  'amazonlib' in options.targets:
    _include_lighttpd_web_server = 0

if include_options.iptuner:
    use_interface('ip_tuner')
    use_component('ip_tuner_src')

if include_options.hoteltv:
    _include_hotel_tv = 1

if include_options.apv_v5:
   _apv_v5 = 1
   info("_apv_v5 = 1")
else:
   _apv_v5 = 0
   info("_apv_v5 = 0")
_tivo_app_package = 'vestel_production_v2.504.40.tar.gz'
_tivo_app_package_prestaging = 'vestel_development_v2.504.40.tar.gz'
_tivo_only_one_package = 0

_include_prestaging_package = 0
if _include_lighttpd_web_server and not _tivo_only_one_package and include_options.security_disabled:
    _include_prestaging_package = 1

if external_user and not (vewd_user_build or mstar_user_build or amazon_user_build):
    error('Unsupported external user "{}"'.format(external_user))

if vewd_user_build or mstar_user_build:
    dependent_projects_path = os.path.join(project_root, 'aurora_trunk_dtv/mb180/external_delivery/project')
    _mmsdk_settings_dir = 'mmsdk/output/{}/openssl'.format(_avs_board_type)
    _avs_settings_dir = 'avs/output/{}/openssl'.format(_avs_board_type)
    _avs_ffmpeg_dir = 'ffmpeg/output/{}/openssl'.format(_avs_board_type)
else:
    dependent_projects_path = os.path.join(project_root, 'project')
    if options.no_build:
        _mmsdk_settings_dir = 'mmsdk/{}'.format(_avs_board_type)
        _avs_settings_dir = 'avs/{}'.format(_avs_board_type)
        _avs_ffmpeg_dir = 'ffmpeg/{}'.format(_avs_board_type)
    else:
        _mmsdk_settings_dir = 'mmsdk/output/{}/openssl'.format(_avs_board_type)
        _avs_settings_dir = 'avs/output/{}/openssl'.format(_avs_board_type)
        _avs_ffmpeg_dir = 'ffmpeg/output/{}/openssl'.format(_avs_board_type)

if 'createDist' not in options.targets and include_options.avs:
    _mmsdk_output_dir = 'mmsdk/output/{}/openssl'.format(_avs_board_type)
    # Read Alexa Smart Screen SDK version from project settings 
    _read_mmsdk_settings = os.path.exists(os.path.join(dependent_projects_path, _mmsdk_settings_dir))
    if _read_mmsdk_settings and include_options.avs_mmsdk:
        execfile ('{}/{}/project_settings.py'.format(dependent_projects_path, _mmsdk_settings_dir))
        set_component_config('avs_src', AVS_MMSDK_VERSION = alexa_smart_screen_sdk_version)
        set_component_config('avs_src', AVS_MMSDK_WEBSOCKET_SSL = alexa_smart_screen_websocket_ssl)
        set_component_config('avs_src', AVS_MMSDK_AMBIENT_HOME = 1 if alexa_smart_screen_ambient_home == 'ON' else None)
        set_component_config('avs_src', AVS_MMSDK_AMBIENT_HOME_DEMO = 1 if alexa_smart_screen_ambient_home_demo == 'ON' else None)
        set_component_config('avs_src', AVS_MMSDK_WEBSOCKET_SSL_KEY = '"a35e49b05d5f04d647d1b5426e2dac80"')
        set_component_config('avs_src', AVS_MMSDK_WEBSOCKET_SSL_IV = '"3dac66eb32884e66"')
    else:
        set_component_config('avs_src', AVS_MMSDK_VERSION = None)
        set_component_config('avs_src', AVS_MMSDK_WEBSOCKET_SSL = None)
        set_component_config('avs_src', AVS_MMSDK_AMBIENT_HOME = None)
        set_component_config('avs_src', AVS_MMSDK_AMBIENT_HOME_DEMO = None)
        set_component_config('avs_src', AVS_MMSDK_WEBSOCKET_SSL_KEY = None)
        set_component_config('avs_src', AVS_MMSDK_WEBSOCKET_SSL_IV = None)
        alexa_smart_screen_sdk_version = None
        alexa_smart_screen_sdk_dynamic_libs = ''
        alexa_smart_screen_sdk_static_libs = ''
        alexa_smart_screen_ambient_home = None
        alexa_smart_screen_ambient_home_demo = None

    # Read AVS Device SDK version from project settings 
    _read_avs_settings = os.path.exists(os.path.join(dependent_projects_path, _avs_settings_dir))
    if _read_avs_settings:
        execfile ('{}/{}/project_settings.py'.format(dependent_projects_path, _avs_settings_dir))
        set_component_config('avs_src', AVS_SDK_VERSION = avs_device_sdk_version)
        set_component_config('avs_src', AVS_CAPTION_SUPPORT = avs_device_sdk_caption_support)
    else:
        set_component_config('avs_src', AVS_SDK_VERSION = None)
        set_component_config('avs_src', AVS_CAPTION_SUPPORT = None)
        avs_device_sdk_version = None
        avs_device_sdk_dynamic_libs = ''
        avs_device_sdk_static_libs = ''

    # Read FFMPEG version from project settings 
    _read_avs_ffmpeg_settings = os.path.exists(os.path.join(dependent_projects_path, _avs_ffmpeg_dir))
    if _read_avs_ffmpeg_settings:
        execfile ('{}/{}/project_settings.py'.format(dependent_projects_path, _avs_ffmpeg_dir))
    else:
       avs_ffmpeg_version = None
else:
    avs_device_sdk_version = None
    avs_ffmpeg_version = None
    alexa_smart_screen_sdk_version = None
    alexa_smart_screen_sdk_dynamic_libs = ''
    alexa_smart_screen_sdk_static_libs = ''
    avs_device_sdk_dynamic_libs = ''
    avs_device_sdk_static_libs = ''
    alexa_smart_screen_ambient_home = None
    alexa_smart_screen_ambient_home_demo = None

############################################################################

#if include_options.vewd_sdk420:
#Below version is for MB180 SDK4.20 builds V2
  #  if include_options.g32:
   #     _software_version_hex = '0x02310000'
    #elif include_options.g31:
     #   _software_version_hex = '0x02300000'
#elif include_options.vewd_sdk423:
  #  if include_options.uk2023_legacy_projects:
#Below version is for MB180 SDK4.23 legacy builds V3
       # if include_options.g32:
          #  _software_version_hex = '0x030B0000'
       # elif include_options.g31:
           # _software_version_hex = '0x03110000'
   # else:
#Below version is for TiVo (Voltron) project versioning
      #  if include_options.apv_v5:
           # _software_version_hex = '0x02330000'
       # else:
          #  _software_version_hex = '0x01330000'

if _include_hotel_tv:
    _software_version_hex = '0x072200AB'
else:
    execfile('../generic_software_version.py')

#construct string representing version
ui_software_version_long = long(_software_version_hex, 16)
version_digit_0 = (ui_software_version_long & 0xFF000000) >> 24
version_digit_1 = (ui_software_version_long & 0x00FF0000) >> 16
version_digit_2 = (ui_software_version_long & 0x0000FF00) >> 8
version_digit_3 = ui_software_version_long & 0x000000FF

_ui_software_version = "V." + str(version_digit_0) + "." + str(version_digit_1) + "." + str(version_digit_2) + "." + str(version_digit_3)

# Set the device model variable (we will determine it from the directory name this conf.py is in)

if include_options.g32:
    mb_name = 'mb180'
    _device_model = 'mb180'
    config_directory = 'config_mb180/defaults'
    #temporary solution for mb180 config directory issue
    use_interface('config_mb181/defaults')

if include_options.g31:
    mb_name = 'mb180'
    mb_name_sw_config_tool = mb_name + 'G31'
    _device_model = 'mb180'
    config_directory = 'config_mb180/defaults'
    #temporary solution for mb180 config directory issue
    use_interface('config_mb181/defaults')
    if include_options.vewd_sdk423 and not include_options.uk2023_legacy_projects:
        # Set WhoAmI identifier str as VLTRON for Vewd 4.23 G31 TiVo project
        _receiver_identifier_str = 'VLTRON'
    else:
        # Set WhoAmI identifier str as QUIGON for G31
        _receiver_identifier_str = 'QUIGON'

if include_options.vewd_browser and not include_options.connected:
    error("You need to specify --options=connected for Vewd browser support", error_status=1)

############################################################################
hosttype = os.environ.get('OSTYPE', 'linux')

toolchain = 'mstar-arm-5.5'
set_platform(toolchain + '-' + hosttype)

set_globals(warnlevel = 'normal')

noclnx_flag = 0
if not include_options.clnx or not mstar_user_build:
    os.environ['NO_COLINUX'] = "1"
    noclnx_flag = 1

# If MSTAR_BASE is not defined, these toolchain directories are checked in
# order; the first that exists is used.

toolchain_dirs = (
    "/tools/arm/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi-MTK",
    "/mtkoss/gnuarm/gcc-arm-linux-gnu-5.5.0-ubuntu/x86_64",
    "/opt/toolchains/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabi-MTK",
    )

# Check if the value of the MSTAR_BASE environment variable isn't one of the
# values in toolchain_dirs.  This can happen if this conf.py is invoked via a
# pconf.py that has a dependent project which sets this variable for a
# different toolchain (for example, the mb180 pconf invokes mb181_ecp_ta as a
# dependent project, and the latter uses a 4.9.4 toolchain).
#
# If this is the case, the MSTAR_BASE environment variable must be corrected,
# as the mstar-arm-5.5 platform makefile uses this environment variable to
# select the compiler.

try:
    toolchain_dir=os.environ['MSTAR_BASE']
    info("MSTAR_BASE already \"{0}\" in mb180 conf.py".format(toolchain_dir))
    if toolchain_dir not in toolchain_dirs:
        del os.environ['MSTAR_BASE']
        info("removed MSTAR_BASE from environ mapping in mb180 conf.py"
             " as its value is incorrect")
except KeyError:
    pass

try:
        toolchain_dir=os.environ['MSTAR_BASE']
except KeyError:
    for dir in toolchain_dirs:
        if os.path.exists(dir):
            toolchain_dir = dir
            break
    os.environ['MSTAR_BASE'] = toolchain_dir

TOOLCHAIN_PATH=toolchain_dir + '/bin'
verbose('note: Using toolchain in ' + toolchain_dir)

if not os.environ['PATH'].startswith(TOOLCHAIN_PATH):
    os.environ['PATH'] = '%s:%s' % (TOOLCHAIN_PATH, os.environ['PATH'])

TOOLCHAIN_GCC_PREFIX = 'arm-linux-gnueabi-'
os.environ['MTK_TOOLCHAIN_GCC_PREFIX'] = TOOLCHAIN_GCC_PREFIX
os.environ['MTK_TOOLCHAIN_PATH'] = toolchain_dir

############################################################################
if 'build_dynamic_ui' not in options.targets and 'createDist' not in options.targets:
    _use_dial_170 = not include_options.dial20
    _use_dial_200 = include_options.dial20
    console_in_root = 0  # 0-> non-root, 1-> root
    root_user_enable = 0 # 0-> non-root, 1-> root
    _spi_flag_ini_path='/conf/spi_flag'
    _playready30_cer_path=certificate_path + '/PLAYREADY30'
    _youtube_cer_path=certificate_path + '/youtube'
    _widevine_cer_path=certificate_path + '/WVCENC'
    _hdcp2_part_path=certificate_path
    _netflix_cer_path=certificate_path + '/netflix50'
    _teerfs_path=certificate_path + '/tee'
    _ciecp_cer_path=certificate_path + '/ves_ci' 
    _netflix_ro_path='/vendor/applications/Netflix'  # NETFLIX_RO_PATH
    _netflix_conf_pub_rw_path='/conf/app_pub/netflix' # NETFLIX_CONF_PUB_RW_PATH
    _netflix_conf_pri_rw_path='/conf/app_pri/netflix' # NETFLIX_CONF_PRI_RW_PATH
    _netflix_data_pub_rw_path='/AppData/app_pub/netflix' # NETFLIX_DATA_PUB_RW_PATH
    _playfi_ro_path='/vendor/applications/playfi'  # PLAYFI_RO_PATH
    _cobalt_ro_path='/vendor/applications/cobalt'   #COBALT_RO_PATH
    _cobalt_conf_pub_rw_path='/conf/app_pub/cobalt' #COBALT_CONF_PUB_RW_PATH
    _cobalt_conf_pri_rw_path='/conf/app_pri/cobalt' #COBALT_CONF_PRI_RW_PATH
    _dplus_ro_path='/vendor/applications/dplus'   #DPLUS_RO_PATH
    _dplus_conf_pub_rw_path='/conf/app_pub/dplus' #DPLUS_CONF_PUB_RW_PATH
    _dplus_conf_pri_rw_path='/conf/app_pri/dplus' #DPLUS_CONF_PRI_RW_PATH
    _udhcpc_conf_pub_rw_path='/conf/app_pub/udhcpc' #UDHCPC_CONF_PUB_RW_PATH
    _udhcp_conf_pub_rw_path='/conf/app_pub/udhcp' #UDHCP_CONF_PUB_RW_PATH
    _dial_ro_path='/vendor/applications/dial'  # DIAL_RO_PATH
    _dial_conf_pub_rw_path='/conf/app_pub/dial'  # DIAL_CONF_PUB_RW_PATH
    _dial_data_pub_rw_path=''  # DIAL_DATA_PUB_RW_PATH
    _apm_ro_appcfg_path='/etc/vendor' # APM_RO_APPCFG_PATH
    _apm_ro_path='/vendor/applications/apm' # APM_RO_PATH
    _apm_conf_pub_rw_path='/conf/app_pub/apm' # APM_CONF_PUB_RW_PATH
    _apm_data_pub_rw_path='' # APM_DATA_PUB_RW_PATH
    _amazon_ro_path='/vendor/applications/Amazon' # AMAZON_RO_PATH
    _amazon_cacert_path='/vendor/applications/certificates' # AMAZON_CA_PATH
    _amazon_conf_pub_rw_path='/conf/app_pub/Amazon'  # AMAZON_CONF_PUB_RW_PATH
    _amazon_conf_pri_rw_path='/conf/app_pri/Amazon'   # AMAZON_CONF_PRI_RW_PATH
    _amazon_data_pub_rw_path='/buffer/Amazon'   # AMAZON_DATA_PUB_RW_PATH
    _vestel_conf_pub_rw_path='/conf/app_pub/vestel' # Vestel_CONF_PUB_RW_PATH
    _vestel_conf_pri_rw_path='/conf/app_pri/vestel' # Vestel_CONF_PRI_RW_PATH
    _networkservice_ro_path='/vendor/applications/netwrokservice' # NETWORKSERVICE_RO_PATH
    _voldservice_ro_path='/vendor/applications/voldservice' # VOLDSERVICE_RO_PATH
    _systemservice_ro_path='/vendor/applications/systemservice' # SYSTEMSERVICE_RO_PATH
    _mountservice_ro_path='/vendor/applications/mountservice' # MOUNTSERVICE_RO_PATH
    _resetservice_ro_path='/vendor/applications/resetservice' # RESETSERVICE_RO_PATH
    _resetnetflixservice_ro_path='/vendor/applications/resetnetflixservice' # RESETNETFLIXSERVICE_RO_PATH

    ###############################################################################
    _connected = 0

    _include_options_extended_logging = 0
    if include_options.extended_logging:
        _include_options_extended_logging = 1

    if include_options.aprime:
        _include_AMZ_support = 1
    else:
        _include_AMZ_support = 0

    if include_options.vewd_browser or include_options.connected:
        _connected = 1

    if include_options.g10:
        if include_options.connected:
            _mstar_bsp = 'G10_connected'
            include_options.nand512 = 1
            connected_str = '_connected'
            if include_options.aprime:
               _mstar_cusInfo = 'G10_connected_amazon'
            else:
               _mstar_cusInfo = 'G10_connected'
        else:
           _mstar_bsp = 'G10'
           _mstar_cusInfo = 'G10'
    elif include_options.g36:
            _mstar_chip = 'G36'
            _mstar_bsp = 'G3X'
            connected_str = '_connected'
            _mstar_cusInfo = 'G36'
            mmap_header = 'MB181'
    elif include_options.g32:
            _mstar_chip = 'G32'
            _mstar_bsp = 'G3X'
            connected_str = '_connected'
            _mstar_cusInfo = 'G32'
            mmap_header = 'MB180'
            _model_year = '2020'
    elif include_options.g31:
            _mstar_chip = 'G31'
            _mstar_bsp = 'G31'
            connected_str = '_connected'
            _mstar_cusInfo = 'G31'
            mmap_header = 'MB180G31'
            if include_options.vewd_sdk423 and not include_options.uk2023_legacy_projects:
                _model_year = '2023'
            else:
                _model_year = '2020'
    else:
         _mstar_bsp = 'D8S'
         _mstar_cusInfo = 'G10'

    if include_options.mmap_0p5G:
        _mmap_config = '512'
    elif include_options.mmap_768M:
        _mmap_config = '256_512'
    elif include_options.mmap_10G:
        _mmap_config = '512_512'
    elif include_options.mmap_125G:
        _mmap_config = '256_512_512'
    elif include_options.mmap_15G:
        _mmap_config = '256_512_256_512'
    else:
        _mmap_config = '256_512'

    if 'nfs' in options.targets or ('D' in canonical_variant):
        nfsbuild = 'nfs'
    else:
        nfsbuild = 'not_nfs'

    if include_options.hard_float:
        os.environ['FLOATING_POINT_FLAG'] = '-mfpu=neon -mfloat-abi=softfp'
        floating_point = 'hard_float'
        _fpu = 1
    else:
        os.environ['FLOATING_POINT_FLAG'] = '-mfpu=neon -mfloat-abi=soft'
        floating_point = 'soft_float'
        _fpu = 0

    _include_html_applications = 1
    if not include_options.vewd_browser or mstar_user_build or not (include_options.nand512 or include_options.emmc) or vewd_user_build or amazon_user_build:
        _include_html_applications = 0

    _include_html_app_sdk_tests = 0
    if _include_hotel_tv:
        _include_html_app_sdk_tests = 1

    _build_vewd_plugins = 1
    if not include_options.vewd_browser or mstar_user_build:
        _build_vewd_plugins = 0

    if include_options.target_hd:
        _osd_resolution = 2
    elif include_options.target_sd:
        _osd_resolution = 1
    elif include_options.target_fhd:
        _osd_resolution = 3
    else:
        _osd_resolution = 1

    _mmsdk = 1

    _include_options_apm = 0
    if include_options.apm:
        _include_options_apm = 1

    if include_options.widevine:
        _include_widevine_support =1

    if include_options.vmx_web:
        _include_verimatrix_support = 1

    if not mstar_user_build and not vewd_user_build and include_options.vewd_browser and not amazon_user_build:
        if include_options.vewd_sdk420 and include_options.hbbtv:
            _build_ciplus_drm_backend = 1

    _include_options_nghttp2 = 0
    if include_options.nghttp2:
        _include_options_nghttp2 = 1

    _include_options_avs = 0
    _avs_caption_support = 0
    if include_options.avs:
        _include_options_avs = 1
        if _read_avs_settings:
            if avs_device_sdk_caption_support != None:
                _avs_caption_support = 1
        else:
            warning("Warning: --options=avs requires 'project/avs' to be built.")

    _include_options_avs_mmsdk = 0
    _avs_mmsdk_ambienthome_support = 0
    _avs_mmsdk_ambienthome_demo_support = 0
    if include_options.avs_mmsdk:
        _include_options_avs_mmsdk = 1
        if alexa_smart_screen_ambient_home == 'ON':
            _avs_mmsdk_ambienthome_support = 1
        if alexa_smart_screen_ambient_home_demo == 'ON':
            _avs_mmsdk_ambienthome_demo_support = 1
        if not _read_mmsdk_settings:
            warning("Warning: --options=mmsdk requires 'project/mmsdk to be built.")

    if include_options.xpca:
        _include_xpca = 1

    _include_options_aws_iot = 0
    if include_options.aws_iot:
        _include_options_aws_iot = 1

    project_directory = os.path.join(project_root, 'aurora_trunk_dtv')
    verbose("Project directory: %s" % project_directory)
    project_release_dir = os.path.join(project_directory, "mb180/environment/release")
    project_config_dir = os.path.join(project_root, config_directory)
    project_build_dir = os.path.join(project_directory, "mb180")

    mmap_path = "%s/environment/distribution/vendor/config/mbrg_mmap_%s_%s_IOMMU.h" % ( project_build_dir, mmap_header, _mmap_config) 
    directfbrc_path = "%s/%s/mstar/dfb/directfbrc_%s" % (project_root, ice_source_directory, _mstar_chip)
    info("mmap_path: %s" % (mmap_path))
    info("directfbrc_path: %s" % (directfbrc_path))
    special_build(
            'check_dfb_frame_buffer',
            command = ("python %s/scripts/check_dfb_frame_buffer.py %s %s"% (project_build_dir, mmap_path, directfbrc_path),),
            required_by = cbuild_first,
            phony = 1,
        )

    if include_options.g31:
        directfbrc_oled_path = "%s/%s/mstar/dfb/directfbrc_%s_OLED" % (project_root, ice_source_directory, _mstar_chip)
        info("directfbrc_oled_path: %s" % (directfbrc_oled_path))
        special_build(
            'check_dfb_frame_buffer',
            command = ("python %s/scripts/check_dfb_frame_buffer.py %s %s"% (project_build_dir, mmap_path, directfbrc_oled_path),),
            required_by = cbuild_first,
            phony = 1,
        )

    if include_options.vewd_browser:
        if include_options.vewd_sdk420:
            vewd_sdk_package_file_name = 'vewd-core-sdk-4.20.5.61.LIMA.224-linux_MB181-release_LIMA-443_EB3.tar.gz'
            vewd_sdk_package_location = '3rd_party/vewd/sdk420/ARM'
            vewd_sdk_dist_dir = os.path.join('work/vewd/sdk420/ARM', vewd_sdk_package_file_name.rsplit(".", 2)[0])
            vewd_sdk_package_file = os.path.join(project_root, vewd_sdk_package_location, vewd_sdk_package_file_name)
        elif include_options.vewd_sdk423:
            vewd_sdk_package_file_name = 'vewd-core-sdk-4.23.2.96.LIMA2.106-linux_MB180-release.tar.gz'
            vewd_sdk_package_location = '3rd_party/vewd/sdk423/ARM'
            vewd_sdk_dist_dir = os.path.join('work/vewd/sdk423/ARM', vewd_sdk_package_file_name.rsplit(".", 2)[0])
            vewd_sdk_package_file = os.path.join(project_root, vewd_sdk_package_location, vewd_sdk_package_file_name)
        else:
            error("Older versions of Vewd are not supported!!!", error_status=1)
        vewd_hbbtv_dist_dir = ''
        vewd_platform = ''
        vewd_clib = ''
        vewd_module = 'vewd_integration'
        vewd_font_location = '3rd_party/fonts/OpenSans'
        vewd_drm_backend_dir = vewd_module + '/drm_backends'
        nss_lib_dir = 'nss-3.73'

    if include_options.nand512:
        _storage = 'nand'
        _storage_size = 512
    elif include_options.emmc:
        _storage = 'emmc'
        _storage_size = 4

    if include_options.tee:
        _tee_option = 'tee'
    else:
        _tee_option = 'no_tee'

    if include_options.widi:
        _include_widi_support = 1
    if include_options.ice_tests or include_options.challenger:
        _include_widi_support = 0

    ###############################################################################
    
    release_all_command = (
            "rm -rf %s/environment/*.tbl*" % project_build_dir,
            "cd %s/environment/; cp -f config.%s.%s.%s_%d%s .config" % ( project_build_dir, _mstar_chip, _mmap_config, _storage, _storage_size, _disable_dsr) ,
            "cp -f %s/environment/distribution/LINUX_ROOTFS/Utils/mem_para_%s.sh %s/environment/distribution/LINUX_ROOTFS/Utils/mem_para.sh" % (project_build_dir , _mstar_chip , project_build_dir ),
            "cd %s/environment; make clean; ./mkrootfs.sh; make release-all" % project_build_dir,
            #"sed -i 's|PLAYREADY_INFO_PATH|%s|g' %s/vendor/config/PlayReady/drminfo.ini" % (_playready_info_path, project_release_dir),
    )

    if _include_AMZ_support :
        if root_user_enable: #system is root
            release_all_command += (
                #"sed -i 's/\(^\ \ \ \ APM_RO_PATH\/bigbang\) ---enable_no_root/\\1/g' %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"sed -i 's/\(^\ \ \ \ APM_RO_PATH\/bigbang\) ---start_service svcmgr/\\1/g' %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"sed -i 's/\(^\ \ \ \ APM_RO_PATH\/bigbang\) ---name APPMGR/\\1/g' %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"sed -i 's/#APM_RO_PATH\/svcmgr&/APM_RO_PATH\/svcmgr\&/g' %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
            )
        release_all_command += (
            "sed -i 's|APM_RO_PATH|%s|g' %s/ROOTFS/etc/init.d/rcS" % (_apm_ro_path, project_release_dir),
            "sed -i 's|APM_RO_APPCFG_PATH|%s|g' %s/ROOTFS/etc/init.d/rcS" % (_apm_ro_appcfg_path, project_release_dir),
        )
        if not console_in_root: # console is non-root
            release_all_command += (
                #Enable trap for each signal but in sysinit stage ctrl+c is do not work
                #"sed -i '2itrap \"\" SIGINT SIGQUIT SIGTSTP SIGHUP SIGKILL SIGTERM SIGSTOP' %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                # in case of switch user, runs bigbang in background
                #"sed -i 's/\(app.cfg\$\)/\\1\&/g' %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                # login console as non-root user
                #"sed -i '/#switch_console_to_nonroot.*/aecho \"switch to %s\"\\nsu -l %s' %s/ROOTFS/etc/init.d/rcS" % (_login_user, _login_user, project_release_dir),
                #Disable trap for each signal
                #"echo 'trap SIGINT' >> %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"echo 'trap SIGQUIT' >> %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"echo 'trap SIGTSTP' >> %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"echo 'trap SIGHUP' >> %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"echo 'trap SIGKILL' >> %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"echo 'trap SIGTERM' >> %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
                #"echo 'trap SIGSTOP' >> %s/ROOTFS/etc/init.d/rcS" % project_release_dir,
            )
        release_all_command += (
            # abandon ttyS0 restarts (it would login as root after restart)
            "sed -i '/ttyS0/d' %s/ROOTFS/etc/inittab" % project_release_dir,
            # create non-root user for console
        )


    special_build(
        'update_mode',
        command = (
            "perl -i -pe 's/^cpio -ivd/cpio -ivd/;' %s/environment/mkrootfs.sh" % project_build_dir,
            "perl -i -pe 's/^chmod /chmod /;' %s/environment/mkrootfs.sh" % project_build_dir,
            "chmod -R a+rwx %s/environment" % project_build_dir,
        ),
        required_by = cbuild_first,
        requires = pre_order,
        phony = True,
    )
    pre_order += ' update_mode'

    if include_options.cleanenv:
        special_build(
            'release_all',
            command = (
                release_all_command
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )
        pre_order += ' release_all'

    copy_folder_script = "copy_folders.sh"
    copy_folder_cmd = 'cd %s/external_delivery; chmod a+x %s; ./%s' %(project_build_dir,copy_folder_script, copy_folder_script)
    if options.external_user:
        special_build(
            'copy_folders',
            command = (copy_folder_cmd,
            ),
            required_by = 'CBUILD_FIRST',
            phony = True
            )

    if _include_extended_logging:
        use_interface('3rd_party/pubsub')
        use_interface('project/pubsub')
        pubsub_tarfile = 'pubsub-mb181.tar.gz'
        pubsub_tmp_directory = '$(derived_dir)/libpubsub'
        special_build(
            'setup_pubsub',
            command = (
                "rm -rf {}".format(
                    pubsub_tmp_directory
                    ),
                "mkdir -p {}".format(
                    pubsub_tmp_directory
                    ),
                "tar xf {}/{} -C {}".format(
                    os.path.join(project_root, "project", "pubsub"),
                    pubsub_tarfile,
                    pubsub_tmp_directory
                    ),
                "cp -f {}/MB180_service.key {}/".format(
                    os.path.join(project_root, "project", "pubsub"),
                    os.path.join(project_release_dir, "vendor")
                    ),                    
             ),
             required_by = cbuild_first,
             phony = 1,
    )

    opapp_tmpfs_directory = os.path.join(project_release_dir, 
                                         '.' + _opapp_configuration_dir_path.strip('"'))
    special_build(
        'clear_opapp',
        command = ('rm -rf ' + opapp_tmpfs_directory),
        required_by = cbuild_first,
        phony = 1,
    )

    preinstalled_opapp_dir = os.path.join(project_root, 'opapp', 'internal')
    if include_options.opapp and os.path.isdir(preinstalled_opapp_dir):
        special_build(
           'setup_default_opapp',
            command = (
                'mkdir -p {}'.format(opapp_tmpfs_directory),
                'cp -prL {} {}'.format(
                    preinstalled_opapp_dir,
                    opapp_tmpfs_directory)
             ),
             required_by = cbuild_first,
             phony = 1,
        )

    if include_options.opapp:
        certificate_source = os.path.join(project_config_dir, 'opapp')
        certificate_dest = opapp_tmpfs_directory
        operators = ['hdplus','irdeto','molotov','twise','digitaluk']
        if include_options.security_disabled:
            operators.append('test')

        terminal_source = os.path.join(certificate_source, 'terminal')
        terminal_folders = ['production']
        if include_options.security_disabled:
            terminal_folders.append('test')

        for terminal_folder in terminal_folders:
            special_build(
                'setup_opapp_certificates_terminal_{}'.format(terminal_folder),
                command = (
                    'mkdir -p {}'.format(certificate_dest),
                    'mkdir -p {}'.format(os.path.join(certificate_dest, 'terminal')),
                    'cp -rL {} {}'.format(
                        os.path.join(certificate_source, 'terminal', terminal_folder),
                        os.path.join(certificate_dest, 'terminal'))
                ),
                required_by = cbuild_first,
                phony = 1,
            )

        for operator in operators:
            special_build(
                'setup_opapp_certificates_{}'.format(operator),
                command = (
                    'mkdir -p {}'.format(
                        os.path.join(certificate_dest, 'operators')
                    ),
                    'cp -rL {} {}'.format(
                        os.path.join(certificate_source, 'operators', operator),
                        os.path.join(certificate_dest, 'operators')
                    )
                ),
                required_by = cbuild_first,
                phony = 1,
            )

    # Add cabot test harness tls self signed ca certificate to Browser certificate db
    if include_options.cabot_test_harness_ca_cert:
        browser_certificate_dir = os.path.join(project_release_dir, 'vendor', 'browser_home', 'certificates')
        special_build(
            # Use project_release_dir (e.g. aurora_trunk_dtv/mb230/tmp_rootfs)
            # as this will be copied to where needed
            'setup_opapp_test_harness_ca_cert',
            command = (
                "{}/aurora_trunk_dtv/scripts/update_certificate_db_test_harness.sh {} {}".format(
                    project_root,
                    project_root,
                    browser_certificate_dir,
                    ),
                ),
            required_by = 'setup_vewd',
            requires = 'create_certificate_db', # This must create db first
            phony = 1
        )
        
    if include_options.opapp:
        # Acquire all OpApps that can be manually installed on platform as required.
        use_interface('3rd_party/opapp')

    if include_options.vewd_browser:
        special_build(
            'setup_vewd',
            required_by = 'CBUILD_FIRST',
            requires = 'setup_browser_fonts copy_browser_libs create_certificate_db copy_vewd_json',
            phony = True
        )

        if include_options.vewd_sdk422 or include_options.vewd_sdk423:
            plugins_script_dir = os.path.join(project_directory, 'scripts')
        else:
            plugins_script_dir = os.path.join(project_build_dir, 'scripts')

        special_build(
            'build_vewd_plugins',
            command = (
                ("if [ {build_plugins:d} -eq 1 ]; then export PATH=${{PATH}}:{toolchain_dir}/bin; "
                    "{build_plugins_script} "
                    "{project_root} {vewd_sdk_dist_dir} {browser_dir} {browser_home} "
                    "{variant_assert:d} {variant_trace:d} {variant_verify:d} {derived_dir} "
                    "{compiler};fi"
                 ).format(
                    build_plugins=_build_vewd_plugins,
                    toolchain_dir=toolchain_dir,
                    build_plugins_script=os.path.join(plugins_script_dir, 'build-vewd-plugins.sh'),
                    project_root=project_root,
                    vewd_sdk_dist_dir=vewd_sdk_dist_dir,
                    browser_dir=os.path.join(project_root, vewd_sdk_dist_dir, 'browser_dir'),
                    browser_home=os.path.join(project_root, vewd_sdk_dist_dir, 'browser_home'),
                    variant_assert='A' in canonical_variant,
                    variant_trace='T' in canonical_variant,
                    variant_verify='V' in canonical_variant,
                    derived_dir='$(derived_dir)',
                    compiler=''
                ),
            ),
            requires = 'extract_vewd',
            phony = 1
        )

        special_build(
            'update_browser_ui_source',
            command = (
                 "sed -i 's/\"tabs_limit\":.*/\"tabs_limit\": 2,/' %s/settings.json" % os.path.join(vewd_sdk_dist_dir, "browser_dir/browser_ui")
            ),
            requires = 'extract_vewd',
            phony = 1
        )

        special_build(
            'create_certificate_db',
            command = (
                "%s/scripts/create-certificate-db.sh %s %s %s %s" %  (
                    project_build_dir,
                    project_root,
                    config_directory,
                    os.path.join(vewd_sdk_dist_dir, "certificates"),
                    options.clean_all,
                    ),
                "cp -rf %s/certificates %s/vendor/browser_home" % (vewd_sdk_dist_dir, project_release_dir),
                ),
            requires = 'copy_vewd_dirs',
            phony = 1
        )

        special_build(
            'copy_vewd_json',
            command = (
                "if [ -d %s/vewd_json ] ;then cp -rf %s/vewd_json/config_MTK.json %s/vendor/browser_home/config.json; fi" % (
                    os.path.join(project_root, vewd_module),
                    os.path.join(project_root, vewd_module),
                    project_release_dir),
                ),
            requires = 'copy_vewd_dirs',
            phony = 1
        )

        # Define the options for the call to update_vewd_json.py.
        vewd_json_update_options = []

        special_build(
            'update_vewd_json',
            command = (
                ('python {path}/update_vewd_json.py '
                    '--input {filename} --output {filename} {options}'
                 ).format(
                    path=os.path.join(project_directory, "scripts"),
                    filename='{path}/vendor/browser_home/vewd.json'.format(path=project_release_dir),
                    options=' '.join(vewd_json_update_options))
            ),
            requires = 'copy_vewd_json',
            phony = 1
        )

        special_build(
            'copy_user_js',
            command = (
                "if [ -d %s/userjs ] ;then cp -rf %s/userjs/*.user.js %s/browser_dir/userjs; fi" % (
                    os.path.join(project_root, vewd_module),
                    os.path.join(project_root, vewd_module),
                    os.path.join(project_root,
                    vewd_sdk_dist_dir)),
                ),
            requires = 'extract_vewd',
            phony = 1
        )

        special_build(
            'copy_browser_libs',
            command = (
                "cp -a  %s/* %s/" % (
                    os.path.join(project_root, vewd_sdk_dist_dir, "lib"),
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                "if [ -d '%s' ]; then cp -a %s/* %s/; fi" % (
                    os.path.join(project_root, "3rd_party/libmpg123/lib"),
                    os.path.join(project_root, "3rd_party/libmpg123/lib"),
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                "rm %s/libomi_components.a" % os.path.join(project_release_dir, "vendor/lib"),
                "if [ %s = 'None' ]; then rm -f %s/libnative_egl.so; fi" % (
                    mstar_user_build,
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                "rm %s/libjspp.a" % os.path.join(project_release_dir, "vendor/lib"),
                "cd %s && rm libfreeblpriv3.so libfreebl3.so libnspr4.so libnsspem.so libnss3.so libnssckbi.so libnssdbm3.so libnssutil3.so libplc4.so libplds4.so libsmime3.so libsoftokn3.so libsqlite3.so libssl3.so" % os.path.join(project_release_dir, "vendor/lib"),
                "cp --remove-destination -a %s/* %s/" % (
                    os.path.join(project_root, vewd_sdk_dist_dir, os.path.join("utils", nss_lib_dir, "lib")),
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                    "chmod +w %s/*.so" % (os.path.join(project_release_dir, "vendor/lib"))
                ),
            requires = 'extract_vewd',
            phony = 1
        )

        special_build(
            'setup_browser_fonts',
            command = (
                # Collect all fonts in one place. This makes life easier
                # while creating deliveries for third parties
                "if [ -d %s ]; then cp %s/*.ttf %s/fonts; fi" % (
                    os.path.join(project_root, vewd_font_location),
                    os.path.join(project_root, vewd_font_location),
                    os.path.join(project_root, vewd_sdk_dist_dir)
                    ),
                "if [ -d %s ]; then cp %s/*.ttf %s/fonts; fi" % (
                    os.path.join(project_root, "vewd_common/fonts"),
                    os.path.join(project_root, "vewd_common/fonts"),
                    os.path.join(project_root, vewd_sdk_dist_dir)
                    ),
                "mkdir -p %s/fonts" % os.path.join(project_release_dir, "vendor"),
                # Copy required fonts
                "mkdir -p %s/Fontconfig" % os.path.join(project_release_dir, "vendor/browser_home"),
                # Copy required fonts and export custom font settings
                "rsync -a --exclude=*Gothic* %s/fonts/*.ttf %s/vendor/fonts" % (os.path.join(project_root, vewd_sdk_dist_dir), project_release_dir),
                "if [ -d %s ]; then cp -f %s/fonts.conf %s; fi" % (
                    os.path.join(project_root, 'vewd_common/fonts/font_config'),
                    os.path.join(project_root, 'vewd_common/fonts/font_config'),
                    os.path.join(project_release_dir, "vendor/browser_home/Fontconfig/fonts.conf")
                    ),
                "if [ -s %s ]; then sed -i 's|browser_home|private/browser/browser_home|g' %s; fi" % (
                    os.path.join(project_release_dir, "vendor/browser_home/Fontconfig/fonts.conf"),
                    os.path.join(project_release_dir, "vendor/browser_home/Fontconfig/fonts.conf"),
                    ),
                ),
            requires = 'copy_vewd_dirs',
            phony = 1
        )

        _include_sdk420 = 1 if include_options.vewd_sdk420 else 0
        _include_sdk423 = 1 if include_options.vewd_sdk423 else 0
                    
        special_build(
            'extract_vewd',
            command = (
                # It is important that the recipe for this target is only executed once when a
                # clean has been specified (options.clean_all) to avoid build issues.
                # To help avoid this, do not make this target a dependency of a non-phony target,
                # directly or indirectly.
                "rm -rf %s" % os.path.join(project_root, vewd_sdk_dist_dir) if (options.clean_all and not mstar_user_build) else "",
                "mkdir -p %s" % os.path.join(project_root, vewd_sdk_dist_dir),
                "if [ ! -f '%s/readme.txt' ]; then tar -zxvf %s --directory %s --strip-components=1; fi" % (
                    os.path.join(project_root, vewd_sdk_dist_dir),
                    vewd_sdk_package_file,
                    vewd_sdk_dist_dir
                    ),
                "[ -f '%s/readme.txt' ]" % (os.path.join(project_root, vewd_sdk_dist_dir)),
                # VEWD SDK4.23 requires and provides a newer version of libstdc++ than the one in the toolchain.
                "if [ -d {stdcpp} ]; then cp -P {stdcpp}/libstdc++.so* {lib}/; cd {lib}; ln -sf libstdc++.so.6.0.24 libstdc++.so; fi".format(
                    stdcpp = os.path.join(project_root, vewd_sdk_dist_dir, 'std-c++-libs'),
                    lib = os.path.join(project_root, vewd_sdk_dist_dir, 'lib')
                    ),
                # VEWD SDK4.23 requires but does not provide libcurl.
                "if [ %d -eq 1 ]; then cp -P %s/libcurl.so* %s/lib/; fi" %(
                    _include_sdk423, 
                    os.path.join(project_root, 'project/libcurl/output/mb181/openssl/lib'), 
                    os.path.join(project_root, vewd_sdk_dist_dir)
                    ),
                "if [ -f '%s/fc-cache' ]; then rm %s/fc-cache; fi" % (
                    os.path.join(project_root, vewd_sdk_dist_dir, 'utils/fontconfig-2.11.1/bin'),
                    os.path.join(project_root, vewd_sdk_dist_dir, 'utils/fontconfig-2.11.1/bin')
                    ),
                "if [ -d %s ]; then \
                    if [ %d -eq 1 ]; then cp %s/sdk420/mb180/license_golden.key %s/browser_dir/license.key; fi fi" %(
                    os.path.join(project_root, vewd_module, "watermark"),
                    _include_sdk420, 
                    os.path.join(project_root, vewd_module, "watermark"), 
                    os.path.join(project_root, vewd_sdk_dist_dir)
                    ),
                "if [ -d %s ]; then \
                    if [ %d -eq 1 ]; then cp %s/sdk423/mb18X/license_golden.key %s/browser_dir/license.key; fi fi" %(
                    os.path.join(project_root, vewd_module, "watermark"),
                    _include_sdk423, 
                    os.path.join(project_root, vewd_module, "watermark"), 
                    os.path.join(project_root, vewd_sdk_dist_dir)
                    ),   
                "if [ -d %s/oipf ]; then cp %s/oipf/oipfCapabilities.xml %s/browser_dir; fi" % (
                    os.path.join(project_root, vewd_module),
                    os.path.join(project_root, vewd_module),
                    os.path.join(project_root, vewd_sdk_dist_dir)
                    ),
                ),
            requires = 'copy_folders' if options.external_user else '',
            phony = 1
        )

        special_build(
            'copy_vewd_dirs',
            command = (
                'cp -r %s/ %s' % (
                    os.path.join(project_root, vewd_sdk_dist_dir, 'browser_home'),
                    os.path.join(project_release_dir, "vendor")
                    ),
                '%s/scripts/update-tvstore-json.sh %s' % (
                    project_build_dir,
                    vewd_sdk_dist_dir
                    ),
                'cp -r %s/ %s' % (
                    os.path.join(project_root, vewd_sdk_dist_dir, 'browser_dir'),
                    os.path.join(project_release_dir, "vendor")
                    ),
                ),
                requires = 'copy_user_js build_vewd_plugins update_browser_ui_source',
            phony = 1
        )

        if include_options.use_gles and not mstar_user_build:
            egl_src_dir = os.path.join(project_root, vewd_module, "egl",
                                       "sdk420" if include_options.vewd_sdk420 else "")
            special_build(
                'setup_gles',
                command = (
                    "if [ -f '%s/Makefile' ]; then cd %s && ICE_SOURCE_DIRECTORY=%s make all PROJECTROOT=%s; fi" % (
                        egl_src_dir,
                        egl_src_dir,
                        ice_source_directory,
                        project_root),
                    "if [ -f '%s/libnative_egl.so' ]; then cp -a %s/libnative_egl.so %s/; fi" % (
                        egl_src_dir,
                        egl_src_dir,
                        os.path.join(project_release_dir, "vendor/lib")
                        ),
                ),
                required_by = 'CBUILD_FIRST',
                requires = 'copy_browser_libs build_dfb',
                phony = True,
        )

    if include_options.use_gles or include_options.gles_ui:
        special_build(
            'setup_gles_ui',
            command = (
                "cp -a %s/* %s/" % (
                    os.path.join(project_root, ice_source_directory, "mstar/mali/G3X/hard_float/dynamic"),
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                "ln -sf libMali.so %s/libEGL.so" % (
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                "ln -sf libMali.so %s/libGLESv1_CM.so" % (
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                "ln -sf libMali.so %s/libGLESv2.so" % (
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                "ln -sf libMali.so %s/libmali.so" % (
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
                "ln -sf libMali.so %s/libmali.so.0" % (
                    os.path.join(project_release_dir, "vendor/lib")
                    ),
            ),
            required_by = 'CBUILD_FIRST',
            requires = 'build_dfb',
            phony = True,
    )

    special_build(
        'get_ca_bundle',
        command = ('%s/scripts/get_ca_bundle.sh %s $(derived_dir)' % (project_directory, project_directory)
                  ),
        requires = '',
        )

    if include_options.oprofile:
        special_build(
            'release_oprofile',
            command = (
                "mkdir -p %s/vendor/oprofile" % (project_release_dir),
                "tar xvfz  %s/environment/toolbox/oprofile/oprofile_arm.tar.gz -C %s/vendor/oprofile/" % (project_build_dir, project_release_dir),
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )

    if include_options.valgrind:
        special_build(
            'release_valgrind',
            command = (
                "mkdir -p %s/vendor/valg" % (project_release_dir),
                "tar xvfz  %s/environment/toolbox/valgrind/valgrind_arm.tar.gz -C %s/vendor/valg/" % (project_build_dir, project_release_dir),
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )

    if include_options.use_dfb:
        directfb_dist = os.path.join(ice_source_directory, 'mstar/dfb/directfb-1.4.2m')
        if include_options.g31:
            special_build(
                    'build_dfb',
                    command = (
                        "rm -rf %s" % (directfb_dist),
                        "tar -xzf %s_G3X.tar.gz -C %s/mstar/dfb" % (directfb_dist,ice_source_directory),
                        #"tar -xzf %s/mstar/dfb/zlib-1.2.11_arm.tar.gz -C %s/mstar/dfb" % (ice_source_directory, ice_source_directory),
                        "tar -xzf %s/mstar/dfb/jpeg-9d.tar.gz -C %s/mstar/dfb" % (ice_source_directory, ice_source_directory),
                        "mkdir -p %s/ROOTFS/usr/lib" % (project_release_dir),
                        "find %s/lib/ -name *.a -exec rm {} -fr \;" % (directfb_dist),
                        "find %s/lib/ -name *.la -exec rm {} -fr \;" % (directfb_dist),
                        "find %s/lib/ -name *.o -exec rm {} -fr \;" % (directfb_dist),
                        #"rm -rf %s/lib/libsawman*" % (directfb_dist),
                        "rm -rf %s/lib/directfb-1.4-0/gfxdrivers/*_mstar.so" % (directfb_dist),
                        "cp -Rf %s/lib/* %s/ROOTFS/usr/lib" % (directfb_dist, project_release_dir),
                        #"#cp -Rf %s/mstar/dfb/zlib-1.2.11_arm/lib/* %s/ROOTFS/usr/lib" % (ice_source_directory, project_release_dir),
                        "#mkdir -p %s/vendor/lib" % (project_release_dir),
                        "#cp -Rf %s/lib/* %s/vendor/lib" % (directfb_dist, project_release_dir),
                        "cp -Rf %s/mstar/dfb/jpeg-9d/lib/* %s/ROOTFS/usr/lib" % (ice_source_directory, project_release_dir),
                        #"cp -Rf %s/mstar/dfb/zlib-1.2.11_arm/lib/* %s/vendor/lib" % (ice_source_directory, project_release_dir),
                        "cp -f %s/mstar/dfb/directfbrc_%s %s/conf/directfbrc" % (ice_source_directory, _mstar_chip, project_release_dir),
                        "cp -f %s/mstar/dfb/directfbrc_%s %s/vendor/directfbrc_default" % (ice_source_directory, _mstar_chip, project_release_dir),
                        "cp -f %s/mstar/dfb/directfbrc_%s_OLED %s/conf/directfbrc_OLED" % (ice_source_directory, _mstar_chip, project_release_dir),
                        "cp -f %s/mstar/dfb/directfbrc_%s_OLED %s/vendor/directfbrc_default_OLED" % (ice_source_directory, _mstar_chip, project_release_dir),
                        "cp -rf %s/mstar/mapi/ldm %s/conf/" % (ice_source_directory, project_release_dir),
                    ),
                    required_by = cbuild_first,
                    requires = pre_order,
                    phony = True,
            )
        else:
            special_build(
                    'build_dfb',
                    command = (
                        "rm -rf %s" % (directfb_dist),
                        "tar -xzf %s_G3X.tar.gz -C %s/mstar/dfb" % (directfb_dist,ice_source_directory),
                        #"tar -xzf %s/mstar/dfb/zlib-1.2.11_arm.tar.gz -C %s/mstar/dfb" % (ice_source_directory, ice_source_directory),
                        "tar -xzf %s/mstar/dfb/jpeg-9d.tar.gz -C %s/mstar/dfb" % (ice_source_directory, ice_source_directory),
                        "mkdir -p %s/ROOTFS/usr/lib" % (project_release_dir),

			# FIX: quote patterns to avoid shell expansion
                        "find %s/lib/ -name \"*.a\"  -exec rm -f {} \;" % (directfb_dist),
			"find %s/lib/ -name \"*.la\" -exec rm -f {} \;" % (directfb_dist),
                        "find %s/lib/ -name \"*.o\"  -exec rm -f {} \;" % (directfb_dist),

                        #"rm -rf %s/lib/libsawman*" % (directfb_dist),
                        "rm -rf %s/lib/directfb-1.4-0/gfxdrivers/*_mstar.so" % (directfb_dist),
                        "cp -Rf %s/lib/* %s/ROOTFS/usr/lib" % (directfb_dist, project_release_dir),
                        #"#cp -Rf %s/mstar/dfb/zlib-1.2.11_arm/lib/* %s/ROOTFS/usr/lib" % (ice_source_directory, project_release_dir),
                        "#mkdir -p %s/vendor/lib" % (project_release_dir),
                        "#cp -Rf %s/lib/* %s/vendor/lib" % (directfb_dist, project_release_dir),
                        "cp -Rf %s/mstar/dfb/jpeg-9d/lib/* %s/ROOTFS/usr/lib" % (ice_source_directory, project_release_dir),
                        #"cp -Rf %s/mstar/dfb/zlib-1.2.11_arm/lib/* %s/vendor/lib" % (ice_source_directory, project_release_dir),
                        "cp -f %s/mstar/dfb/directfbrc_%s %s/conf/directfbrc" % (ice_source_directory, _mstar_chip, project_release_dir),
                        "cp -f %s/mstar/dfb/directfbrc_%s %s/vendor/directfbrc_default" % (ice_source_directory, _mstar_chip, project_release_dir),
                    ),
                    required_by = cbuild_first,
                    requires = pre_order,
                    phony = True,
            )
        libDFB_ld_flag += ' -L' + os.path.join(project_build_dir, 'environment/release/ROOTFS/usr/lib')
        pre_order += ' build_dfb'
        libDFB += '-ldirectfb '
        libDFB += '-ldirect '
        libDFB += '-lfusion '
        libDFB += '-ljpeg '
    else:
        special_build(
            'build_dfb',
            command = (
                "rm -rf ../%s/mstar/dfb/lib; rm -rf ../%s/mstar/dfb/include" % (ice_source_directory, ice_source_directory),
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )
        pre_order += ' build_dfb'

    if _include_widevine_support:
        widevine_release_dir = "%s/mstar/widevine/widevineCENC_v3" % (ice_source_directory)
        special_build(
            'setup_widevine',
            command = (
                #'if [ ! -d %s/conf/config/widevinecenc ];then mkdir -p %s/conf/config/widevinecenc;fi' % (project_release_dir, project_release_dir),
                #"mkdir -p %s/vendor/widevine/" % (project_release_dir),
                #'cp -a %s/* %s' % (os.path.join(widevine_release_dir, 'lib'),
                #                   os.path.join(project_release_dir, "vendor/widevine")),
                #'cp -a %s/*.so* %s' % (os.path.join(project_root, 'project/openssl/output/mb181/lib'),
                #                   os.path.join(project_release_dir, "vendor/widevine")),
                #'cp -a %s/config.ini %s' % (os.path.join(ice_source_directory, 'mstar/widevine/'),
                #                   os.path.join(project_release_dir, "vendor/config/widevine")),
            ),
        )

        pre_order += ' setup_widevine'

    libAPM = ''
    apm_ld_flags = ''
    if include_options.apm and include_options.aprime:
        special_build(
            'build_apm',
            command = (
                "mkdir -p %s%s" % (project_release_dir,_apm_ro_path),
                "cp %s/mstar/gam/bin/gam %s%s" % (ice_source_directory, project_release_dir, _apm_ro_path),
                "cp %s/mstar/gam/bin/mtk_launcher %s%s" % (ice_source_directory, project_release_dir, _apm_ro_path),
                "cp %s/mstar/gam/lib/*.so %s/vendor/lib" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/apm/G10/appmgr %s%s/" % (ice_source_directory, project_release_dir, _apm_ro_path),
                "cp %s/mstar/apm/G10/bigbang %s%s/" % (ice_source_directory, project_release_dir, _apm_ro_path),
                "cp %s/mstar/apm/G10/svcmgr %s%s/" % (ice_source_directory, project_release_dir, _apm_ro_path),
                'if [ ! -d %s%s ];then mkdir -p %s%s;fi' % ( project_release_dir,_apm_conf_pub_rw_path,project_release_dir,_apm_conf_pub_rw_path),
                'if [ ! -d %s/conf/Customer ];then mkdir %s/conf/Customer;fi' % ( project_release_dir,project_release_dir),
                "cp %s/mstar/apm/G10/*.so %s/vendor/lib" % (ice_source_directory, project_release_dir),
                "rm -fr %s/mstar/msrv_mm_playerinterface/lib" % (ice_source_directory),
                "rm -fr %s/mstar/msrv_mm_playerinterface/include" % (ice_source_directory),
                "tar zxf %s/mstar/msrv_mm_playerinterface/msrv_mm_playerinterface.%s.tar.gz -C %s/mstar/msrv_mm_playerinterface/" % (ice_source_directory, _mstar_bsp, ice_source_directory),
                "cp %s/mstar/msrv_mm_playerinterface/lib/*.so %s/vendor/lib" % (ice_source_directory, project_release_dir),
                "cp -rf %s/mstar/apm/G10/cap_control.ini %s/ROOTFS/etc/" % (ice_source_directory, project_release_dir),
                "sed -i 's|DIAL_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_dial_ro_path, project_release_dir),
                "sed -i 's|AMAZON_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_amazon_ro_path, project_release_dir),
                "sed -i 's|NETFLIX_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_netflix_ro_path, project_release_dir),
                "sed -i 's|APM_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_apm_ro_path, project_release_dir),
                "sed -i 's|NETWORKSERVICE_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_networkservice_ro_path, project_release_dir),
                "sed -i 's|VOLDSERVICE_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_voldservice_ro_path, project_release_dir),
                "sed -i 's|SYSTEMSERVICE_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_systemservice_ro_path, project_release_dir),
                "sed -i 's|MOUNTSERVICE_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_mountservice_ro_path, project_release_dir),
                "sed -i 's|RESETSERVICE_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_resetservice_ro_path, project_release_dir),
                "sed -i 's|RESETNETFLIXSERVICE_RO_PATH|%s|g' %s/ROOTFS/etc/cap_control.ini" % (_resetnetflixservice_ro_path, project_release_dir),
                "cp -fr %s/mstar/ipc_svc/G10_NFX50/*.so %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                "echo -e \"\033[36m Building Netflix \033[0m\" ",
                "mkdir -p %s%s" % (project_release_dir,_netflix_ro_path),
                "mkdir -p %s%s" % (project_release_dir,_playfi_ro_path),
                "mkdir -p %s%s" % (project_release_dir,_cobalt_ro_path),
                "mkdir -p %s%s" % (project_release_dir,_dplus_ro_path),
                "mkdir -p %s%s" % (project_release_dir,_dial_ro_path),
                "mkdir -p %s%s" % (project_release_dir,_amazon_ro_path),
                "mkdir -p %s%s" % (project_release_dir, _netflix_data_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _netflix_conf_pri_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _netflix_conf_pub_rw_path),
                "touch %s%s/netflix_report" % (project_release_dir, _netflix_conf_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _spi_flag_ini_path),
                "cp %s/mstar/mapi/flash/spi_map.ini %s%s " % (ice_source_directory, project_release_dir, _spi_flag_ini_path),
                "mkdir -p %s%s" % (project_release_dir, _cobalt_conf_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _dplus_conf_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _udhcpc_conf_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _udhcp_conf_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _dial_conf_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _amazon_conf_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _amazon_conf_pri_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _vestel_conf_pub_rw_path),
                "mkdir -p %s%s" % (project_release_dir, _vestel_conf_pri_rw_path),
                "mkdir -p %s/%s" % (project_release_dir, _networkservice_ro_path),
                "mkdir -p %s/%s" % (project_release_dir, _voldservice_ro_path),
                "mkdir -p %s/%s" % (project_release_dir, _systemservice_ro_path),
                "mkdir -p %s/%s" % (project_release_dir, _mountservice_ro_path),
                "mkdir -p %s/%s" % (project_release_dir, _resetservice_ro_path),
                "mkdir -p %s/%s" % (project_release_dir, _resetnetflixservice_ro_path),

                "mkdir -p %s/mstar/netflix/tmpFolder" % (ice_source_directory),
                "tar zxvf %s/mstar/netflix/%s/Netflix5.2_OPTEE_PKG.tgz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, _mstar_chip, ice_source_directory),
                "cp %s/mstar/netflix/%s/libstplayer.so  %s/mstar/netflix/tmpFolder/Netflix_PKG/arm.hard_float.debug/lib/libstplayer.so" % (ice_source_directory, _mstar_chip, ice_source_directory),
                #"mkdir -p %s/ROOTFS/application/Netflix/bin/" % (project_release_dir),
                #"cp -f %s/mstar/netflix/tmpFolder/Netflix_PKG/arm.hard_float.debug/bin/version_info %s/ROOTFS/application/Netflix/bin/" %(ice_source_directory, project_release_dir),
                "cp -fr %s/mstar/netflix/tmpFolder/Netflix_PKG/data %s%s/" % (ice_source_directory, project_release_dir, _netflix_data_pub_rw_path),
                "ln -sT %s/netflix_report %s%s/data/netflix_report" %(_netflix_conf_pub_rw_path, project_release_dir, _netflix_data_pub_rw_path),
                "cp -fr %s/mstar/netflix/tmpFolder/Netflix_PKG/arm.hard_float.debug/* %s%s/" % (ice_source_directory, project_release_dir,_netflix_ro_path),
                #"cp -f %s%s/data/etc/conf/oem.xml %s%s/data/etc/conf/oem_default.xml" % (project_release_dir, _netflix_data_pub_rw_path,project_release_dir ,_netflix_data_pub_rw_path),
                #"cp -f %s%s/data/etc/conf/oem.xml %s%s/data/etc/conf/oem_nocache.xml" % (project_release_dir, _netflix_data_pub_rw_path,project_release_dir , _netflix_data_pub_rw_path),
                "cp -f %s/mstar/netflix/%s/NetflixInfo.ini %s%s/data/etc/conf/" % (ice_source_directory, _mstar_chip, project_release_dir,_netflix_data_pub_rw_path),
                "cp -f %s/mstar/netflix/%s/oem.xml %s%s/data/etc/conf/" % (ice_source_directory, _mstar_chip, project_release_dir,_netflix_data_pub_rw_path),
                "cp -f %s/mstar/netflix/%s/resetNetflix.sh %s%s/data/" % (ice_source_directory, _mstar_chip, project_release_dir,_netflix_data_pub_rw_path),
                #"sed -i 's/\\(<disk_cache_capacity.*>\\).*\\(<\\/disk_cache_capacity.*\\)/\\10M\\2/' %s%s/data/etc/conf/oem_nocache.xml" % (project_release_dir,_netflix_data_pub_rw_path),
                #"sed -i 's/\\(<media_cache_capacity.*>\\).*\\(<\\/media_cache_capacity.*\\)/\\10M\\2/' %s%s/data/etc/conf/oem_nocache.xml" % (project_release_dir,_netflix_data_pub_rw_path),
                #"sed -i 's/\\(<dpi_videobufferpoolsize.*>\\).*\\(<\\/dpi_videobufferpoolsize.*\\)/\\1193986560\\2/' %s%s/data/etc/conf/oem_default.xml" % (project_release_dir,_netflix_data_pub_rw_path),

                #"cp -f %s%s/data/etc/conf/oem_default.xml %s%s/data/etc/conf/oem_dv_default.xml" % (project_release_dir,_netflix_data_pub_rw_path, project_release_dir,_netflix_data_pub_rw_path),
                #"cp -f %s%s/data/etc/conf/oem_nocache.xml %s%s/data/etc/conf/oem_dv_nocache.xml" % (project_release_dir,_netflix_data_pub_rw_path, project_release_dir,_netflix_data_pub_rw_path),
                #"sed -i 's/<dpi_video_codecs>.*/<dpi_video_codecs>avc,vp9,vp9-hdr,hevc,dv5,hdr10<\\/dpi_video_codecs>/g' %s%s/data/etc/conf/oem_dv_nocache.xml" % (project_release_dir,_netflix_data_pub_rw_path),
                #"sed -i 's/<dpi_video_codecs>.*/<dpi_video_codecs>avc,vp9,vp9-hdr,hevc,dv5,hdr10<\\/dpi_video_codecs>/g' %s%s/data/etc/conf/oem_dv_default.xml" % (project_release_dir,_netflix_data_pub_rw_path),
                "mkdir -p %s/ROOTFS/usr/lib" % (project_release_dir),
                # ----> libpng <---- #
                "tar zxvf %s/mstar/netflix/%s/libpng_1.2.59.tar.gz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, _mstar_chip, ice_source_directory),
                "cp -fa %s/mstar/netflix/tmpFolder/libpng_1.2.59/lib/libpng12.so* %s/ROOTFS/usr/lib/" % (ice_source_directory, project_release_dir), #TEMP SHOULD BE REMOVED LATER for libfreetype
                # ----> libfreetype <---- #
                "tar zxvf %s/mstar/netflix/%s/freetype-2.8.0.tar.gz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, _mstar_chip, ice_source_directory),
                "cp -fa %s/mstar/netflix/tmpFolder/freetype-2.8.0/libfreetype*so* %s/vendor/lib/" % (ice_source_directory, project_release_dir), #TEMP SHOULD BE REMOVED LATER
                # ----> misc libs <---- #
                "cp -f %s/mstar/app_ipc_interface/G3X/*.so %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                "ln -sT /vendor/lib/libAppIpc.so %s/vendor/lib/libapp_if_rpc.so" % (project_release_dir),
                # ----> removing wrong version libs <---- #
                "rm -fr %s/mstar/netflix/tmpFolder" % (ice_source_directory),
                "cp -fr %s/mstar/netflix/%s/stplayer.config %s/ROOTFS/appcache/STPlayer/" % (ice_source_directory, _mstar_chip, project_release_dir),
                "echo -e \"\033[36m End of building Netflix \033[0m\" ",

                "echo -e \"\033[36m Building Cobalt \033[0m\" ",
                "mkdir -p %s/mstar/cobalt/%s/tmpFolder" % (ice_source_directory, _mstar_chip),
                #"tar zxvf %s/mstar/cobalt/%s/cobalt.tar.gz -C %s/mstar/cobalt/%s/tmpFolder/" % (ice_source_directory, _mstar_chip, ice_source_directory, _mstar_chip),
                "if [ %s = 'nand' ];then tar zxvf %s/mstar/cobalt/2023/evergreen/evergreen.tar.gz -C %s/mstar/cobalt/%s/tmpFolder/;fi" % (_storage, ice_source_directory, ice_source_directory, _mstar_chip),
                "if [ %s = 'emmc' ];then tar zxvf %s/mstar/cobalt/2023/evergreen/evergreen.tar.gz -C %s/mstar/cobalt/%s/tmpFolder/;fi" % (_storage, ice_source_directory, ice_source_directory, _mstar_chip),
                "cp -fr %s/mstar/cobalt/2023/evergreen/loader_app %s/mstar/cobalt/%s/tmpFolder/" % (ice_source_directory, ice_source_directory, _mstar_chip),
                "cp -fr %s/mstar/cobalt/2023/evergreen/crashpad_handler %s/mstar/cobalt/%s/tmpFolder/" % (ice_source_directory, ice_source_directory, _mstar_chip),
                "%s/arm-linux-gnueabi-strip %s/mstar/cobalt/%s/tmpFolder/loader_app" % (TOOLCHAIN_PATH,ice_source_directory, _mstar_chip),
                "%s/arm-linux-gnueabi-strip %s/mstar/cobalt/%s/tmpFolder/crashpad_handler" % (TOOLCHAIN_PATH,ice_source_directory, _mstar_chip),
                "cp -fr %s/mstar/cobalt/%s/tmpFolder/* %s%s/" % (ice_source_directory, _mstar_chip, project_release_dir,_cobalt_ro_path),
                "if [ %s = '2020' ];then cp -fr %s/mstar/cobalt/2023/%s/config-2020.ini %s%s/config.ini;fi" % (_model_year,ice_source_directory, _mstar_chip, project_release_dir,_cobalt_ro_path),
                "if [ %s = '2023' ];then cp -fr %s/mstar/cobalt/2023/%s/config-2023.ini %s%s/config.ini;fi" % (_model_year,ice_source_directory, _mstar_chip, project_release_dir,_cobalt_ro_path),
                "cp -fr %s/mstar/cobalt/2023/%s/Cobalt_Keymap.csv %s%s/" % (ice_source_directory, _mstar_chip, project_release_dir,_cobalt_ro_path),
                "cp -fr %s/mstar/cobalt/2023/bin/* %s%s/" % (ice_source_directory, project_release_dir,_cobalt_ro_path),
                "cp -fr %s/mstar/cobalt/2023/lib/* %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                "rm -rf %s/mstar/cobalt/%s/tmpFolder" % (ice_source_directory, _mstar_chip),
                "sed -i 's|SOFTWARE_VERSION|%s|g' %s%s/config.ini" % (_ui_software_version, project_release_dir,_cobalt_ro_path),
                "echo -e \"\033[36m End of building Cobalt \033[0m\" ",

                "echo -e \"\033[36m Building dplus \033[0m\" ",
                "mkdir -p %s/mstar/dplus/%s/tmpFolder" % (ice_source_directory, _mstar_chip),
                #"tar zxvf %s/mstar/dplus/%s/dplus.tar.gz -C %s/mstar/dplus/%s/tmpFolder/" % (ice_source_directory, _mstar_chip, ice_source_directory, _mstar_chip),
                "if [ %s = 'nand' ];then tar zxvf %s/mstar/dplus/%s/dplus.tar.gz -C %s/mstar/dplus/%s/tmpFolder/;fi" % (_storage, ice_source_directory, _mstar_chip, ice_source_directory, _mstar_chip),
                "if [ %s = 'emmc' ];then tar zxvf %s/mstar/dplus/%s/dplus.tar.gz -C %s/mstar/dplus/%s/tmpFolder/;fi" % (_storage, ice_source_directory, _mstar_chip, ice_source_directory, _mstar_chip),
                #"cp -fr %s/mstar/dplus/%s/evergreen/loader_app %s/mstar/dplus/%s/tmpFolder/" % (ice_source_directory, _mstar_chip, ice_source_directory, _mstar_chip),
                #"cp -fr %s/mstar/dplus/%s/evergreen/crashpad_handler %s/mstar/dplus/%s/tmpFolder/" % (ice_source_directory, _mstar_chip, ice_source_directory, _mstar_chip),
                #"%s/arm-linux-gnueabi-strip %s/mstar/dplus/%s/tmpFolder/loader_app" % (TOOLCHAIN_PATH,ice_source_directory, _mstar_chip),
                #"%s/arm-linux-gnueabi-strip %s/mstar/dplus/%s/tmpFolder/crashpad_handler" % (TOOLCHAIN_PATH,ice_source_directory, _mstar_chip),
                "cp -fr %s/mstar/dplus/%s/dss_portal_device.pem %s/mstar/dplus/%s/tmpFolder/release/shield_runtime/shield_agent_data/certs/client/dss_portal_device.pem" % (ice_source_directory, _mstar_chip, ice_source_directory, _mstar_chip),
                "cp -fr %s/mstar/dplus/%s/persona.json %s/mstar/dplus/%s/tmpFolder/release/resource/shared/persona.json" % (ice_source_directory, _mstar_chip, ice_source_directory, _mstar_chip),
                "cp -fr %s/mstar/dplus/%s/tmpFolder/release/* %s%s/" % (ice_source_directory, _mstar_chip, project_release_dir,_dplus_ro_path),
                #"cp -fr %s/mstar/dplus/%s/certificates %s%s/" % (ice_source_directory, _mstar_chip, project_release_dir,_dplus_ro_path),
                #"cp -fr %s/mstar/dplus/%s/config/config.ini %s%s/" % (ice_source_directory, _mstar_chip, project_release_dir,_dplus_ro_path),
                #"cp -fr %s/mstar/dplus/%s/config/dplus_Keymap.csv %s%s/" % (ice_source_directory, _mstar_chip, project_release_dir,_dplus_ro_path),
                "cp %s/mstar/dplus/%s/dplus_deactivate.sh %s%s" % (ice_source_directory, _mstar_chip, project_release_dir, _dplus_ro_path),
                #"cp -fr %s/mstar/dplus/%s/bin/* %s%s/" % (ice_source_directory, _mstar_chip, project_release_dir,_dplus_ro_path),
                #"cp -fr %s/mstar/dplus/%s/lib/* %s/vendor/lib/" % (ice_source_directory, _mstar_chip, project_release_dir),
                #"rm -rf %s/mstar/dplus/%s/tmpFolder" % (ice_source_directory, _mstar_chip),
                #"sed -i 's|SOFTWARE_VERSION|%s|g' %s%s/config.ini" % (_ui_software_version, project_release_dir,_dplus_ro_path),   @@@ TODO: should use ini to get this value
                "echo -e \"\033[36m End of building dplus \033[0m\" ",

                "echo -e \"\033[36m Building Amazon \033[0m\" ",
                #"rm -rf %s/mstar/amazon/tmpFolder" % (ice_source_directory),
                "mkdir -p %s/mstar/amazon/tmpFolder" % (ice_source_directory),
                "if [ %d -eq 1 ];then tar zxvf %s/mstar/amazon/AVPK_V5/avpk.tar.gz -C %s/mstar/amazon/tmpFolder/;fi" % (_apv_v5, ice_source_directory, ice_source_directory),
                "if [ %d -eq 1 ];then cp %s/mstar/amazon/AVPK_V5/%s/amazon_keymap.ini %s/mstar/amazon/tmpFolder/;fi" % (_apv_v5, ice_source_directory, _mstar_chip, ice_source_directory),
                "if [ %d -eq 1 ];then cp %s/mstar/amazon/AVPK_V5/%s/config.ini %s/mstar/amazon/tmpFolder/;fi" % (_apv_v5, ice_source_directory, _mstar_chip, ice_source_directory),
                "if [ %d -eq 1 ];then cp %s/mstar/amazon/AVPK_V5/%s/Amazon_deactivate.sh %s/mstar/amazon/tmpFolder/;fi" % (_apv_v5, ice_source_directory, _mstar_chip, ice_source_directory),
                "if [ %d -eq 0 ];then tar zxvf %s/mstar/amazon/%s/amazon.tar.gz -C %s/mstar/amazon/tmpFolder/;fi" % (_apv_v5, ice_source_directory, _mstar_chip, ice_source_directory),
                "if [ %d -eq 0 ];then tar zxvf %s/mstar/amazon/%s/amazon_oss_lib.tar.gz -C %s/mstar/amazon/tmpFolder/;fi" % (_apv_v5, ice_source_directory, _mstar_chip, ice_source_directory),
                "if [ %d -eq 0 ];then mkdir %s/mstar/amazon/tmpFolder/common/amazon_ignition/lib;fi" % (_apv_v5, ice_source_directory),
                "if [ %d -eq 0 ];then cp %s/mstar/amazon/tmpFolder/common/lib/* %s/mstar/amazon/tmpFolder/common/amazon_ignition/bin/;fi" % (_apv_v5, ice_source_directory, ice_source_directory),
                # ----> copy oss lib to lib folder<---- #
                "if [ %d -eq 1 ];then %s/arm-linux-gnueabi-strip %s/mstar/amazon/tmpFolder/bin/ignition;fi" %  (_apv_v5,TOOLCHAIN_PATH,ice_source_directory),
                "if [ %d -eq 1 ];then %s/arm-linux-gnueabi-strip %s/mstar/amazon/tmpFolder/lib/*;fi" %  (_apv_v5, TOOLCHAIN_PATH,ice_source_directory),
                "if [ %d -eq 1 ];then mv -f %s/mstar/amazon/tmpFolder/* %s%s;fi"  % (_apv_v5, ice_source_directory, project_release_dir,_amazon_ro_path),
                "if [ %d -eq 0 ];then cp -arf %s/mstar/amazon/tmpFolder/amazon_oss_lib/jpeg_6b/* %s/mstar/amazon/tmpFolder/common/amazon_ignition/lib/;fi" % (_apv_v5, ice_source_directory, ice_source_directory),
                "if [ %d -eq 0 ];then cp -arf %s/mstar/amazon/tmpFolder/amazon_oss_lib/curl_7.59.0/* %s/mstar/amazon/tmpFolder/common/amazon_ignition/lib/;fi" % (_apv_v5, ice_source_directory, ice_source_directory),
                "if [ %d -eq 0 ];then cp -arf %s/mstar/amazon/tmpFolder/amazon_oss_lib/freetype_2.3.9/* %s/mstar/amazon/tmpFolder/common/amazon_ignition/lib/;fi" % (_apv_v5, ice_source_directory, ice_source_directory),
                "if [ %d -eq 0 ];then cp -arf %s/mstar/amazon/tmpFolder/amazon_oss_lib/openssl_1.0.2s/* %s/mstar/amazon/tmpFolder/common/amazon_ignition/lib/;fi" % (_apv_v5, ice_source_directory, ice_source_directory),
                "if [ %d -eq 0 ];then cp -arf %s/mstar/amazon/tmpFolder/amazon_oss_lib/png_1.2.43/* %s/mstar/amazon/tmpFolder/common/amazon_ignition/lib/;fi" % (_apv_v5, ice_source_directory, ice_source_directory),
                "if [ %d -eq 0 ];then cp -arf %s/mstar/amazon/tmpFolder/amazon_oss_lib/zlib_1.2.3/* %s/mstar/amazon/tmpFolder/common/amazon_ignition/lib/;fi" % (_apv_v5, ice_source_directory, ice_source_directory),
                "if [ %d -eq 0 ];then mv -f %s/mstar/amazon/tmpFolder/common/amazon_ignition/* %s%s;fi"  % (_apv_v5, ice_source_directory, project_release_dir,_amazon_ro_path),
                "if [ %d -eq 0 ];then cp %s/mstar/amazon/tmpFolder/common/Amazon_deactivate.sh %s%s;fi" % (_apv_v5, ice_source_directory, project_release_dir, _amazon_ro_path),
                "if [ %d -eq 0 ];then cp %s/mstar/amazon/tmpFolder/common/*.ini %s%s;fi" % (_apv_v5, ice_source_directory, project_release_dir, _amazon_ro_path),
                "if [ %d -eq 0 ];then cp %s/mstar/amazon/%s/amazon_keymap.ini %s%s;fi" % (_apv_v5, ice_source_directory, _mstar_chip, project_release_dir, _amazon_ro_path),
                #"cp %s/mstar/amazon/tmpFolder/common/cacerts.pem %s%s" % (ice_source_directory, project_release_dir, _amazon_cacert_path),
                "rm -rf %s/mstar/amazon/tmpFolder" % (ice_source_directory),
                'if [ ! -d %s/ROOTFS%s ];then mkdir -p %s/ROOTFS%s;fi' % ( project_release_dir,_apm_ro_appcfg_path ,project_release_dir,_apm_ro_appcfg_path),
                "cp %s/mstar/gam/am_config.cfg %s/ROOTFS%s/app.cfg" % (ice_source_directory,project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|DIAL_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_dial_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|DIAL_CONF_PUB_RW_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_dial_conf_pub_rw_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|AMAZON_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_amazon_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|AMAZON_CONF_PUB_RW_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_amazon_conf_pub_rw_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|AMAZON_CA_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_amazon_cacert_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|NETFLIX_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_netflix_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|NETWORKSERVICE_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_networkservice_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|VOLDSERVICE_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_voldservice_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|SYSTEMSERVICE_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_systemservice_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|MOUNTSERVICE_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_mountservice_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|RESETSERVICE_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_resetservice_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "sed -i 's|RESETNETFLIXSERVICE_RO_PATH|%s|g' %s/ROOTFS%s/app.cfg" % (_resetnetflixservice_ro_path, project_release_dir, _apm_ro_appcfg_path),
                "cp %s/mstar/networkservice/release/bin/* %s/%s/" % (ice_source_directory, project_release_dir, _networkservice_ro_path),
                "cp %s/mstar/networkservice/release/lib/*.so %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/voldservice/release/bin/* %s/%s/" % (ice_source_directory, project_release_dir, _voldservice_ro_path),
                "cp %s/mstar/voldservice/release/lib/*.so %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/systemservice/release/bin/* %s/%s/" % (ice_source_directory, project_release_dir, _systemservice_ro_path),
                "cp %s/mstar/systemservice/release/lib/*.so %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/mountservice/release/bin/* %s/%s/" % (ice_source_directory, project_release_dir, _mountservice_ro_path),
                "cp %s/mstar/mountservice/release/lib/*.so %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/resetservice/reset_config.ini %s/%s/" % (ice_source_directory, project_release_dir, _resetservice_ro_path),
                "sed -i 's|AMAZON_CONF_PRI_RW_PATH|%s|g' %s/%s/reset_config.ini" % (_amazon_conf_pri_rw_path, project_release_dir, _resetservice_ro_path),
                "sed -i 's|AMAZON_DATA_PUB_RW_PATH|%s|g' %s/%s/reset_config.ini" % (_amazon_data_pub_rw_path, project_release_dir, _resetservice_ro_path),
                "cp %s/mstar/resetservice/ResetAmazon %s/%s/" % (ice_source_directory, project_release_dir, _resetservice_ro_path),
                "cp %s/mstar/resetnetflixservice/reset_config.ini %s/%s/" % (ice_source_directory, project_release_dir, _resetnetflixservice_ro_path),
                "cp %s/mstar/resetnetflixservice/ResetNetflix %s/%s/" % (ice_source_directory, project_release_dir, _resetnetflixservice_ro_path),
                #---- DSR debug version start ----
                #"if [ %s = '.debug' ];then touch %s/conf/dump_enabled.bin;fi" % (_disable_dsr, project_release_dir),
                "if [ %s = '.debug' ];then touch %s/ROOTFS/bin/selinux.disabled;fi" % (_disable_dsr, project_release_dir),
                "if [ %s = '.debug' ];then cp -f %s/mstar/gam/bin/gam_debug %s%s/gam;fi" % (_disable_dsr, ice_source_directory, project_release_dir, _apm_ro_path),
                "if [ %s = '.debug' ];then cp -f %s/../distribution/LINUX_ROOTFS/SELinux/arm-linux-gnueabi_5.5.0/bin/sush %s/ROOTFS/bin/;fi" % (_disable_dsr, project_release_dir, project_release_dir),
                "if [ %s = '.debug' ];then touch %s/ROOTFS/bin/tivo_debug;fi" % (_disable_dsr, project_release_dir),
                #---- Enable telnetd
                #"if [ %d -eq 1 ]; then touch %s/ROOTFS/bin/Tnetd.enabled;fi" % (_enable_telnetd, project_release_dir),
                #---- DSR debug version end ----
                "echo -e \"\033[36m End of Amazon build \033[0m\" ",
            )
        )
        pre_order += ' build_apm'
        apm_ld_flags = ' -L' + os.path.join(project_build_dir, 'environment/release/vendor/lib')
        libAPM += ' -l'.join([
                                    #'-lAPM',
                                    '-lgam_client',
                                    'mdroid',
                                    'cusinfo',
                                    'msrv_mmplayer_ipc',
                                    'mmplayerservice',
                                    'AppIpc',
                                    'AppIpcService',
                                    'SNApi',
                                    'SNApiService',
                                    'SystemService',
                                    'NetworkService',
                                    'VoldService',
                                    'SystemService',
                                    'MountService',
                                    ])

    if include_options.apm and not include_options.aprime:
        special_build(
            'build_apm',
            command = (
                "cp %s/mstar/apm/G10/appmgr %s/vendor/" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/apm/G10/bigbang %s/vendor/" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/apm/G10/svcmgr %s/vendor/" % (ice_source_directory, project_release_dir),
                'if [ ! -d %s/conf/Customer ];then mkdir %s/conf/Customer;fi' % ( project_release_dir,project_release_dir),
                #"cp %s/mstar/apm/G10/app.cfg %s/conf/Customer/" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/gam/am_config.cfg %s/conf/Customer/app.cfg" % (ice_source_directory, project_release_dir),
                "cp %s/mstar/gam/lib/*.so %s/vendor/lib" % (ice_source_directory, project_release_dir),
                "rm -fr %s/mstar/msrv_mm_playerinterface/lib" % (ice_source_directory),
                "rm -fr %s/mstar/msrv_mm_playerinterface/include" % (ice_source_directory),
                "tar zxf %s/mstar/msrv_mm_playerinterface/msrv_mm_playerinterface.%s.tar.gz -C %s/mstar/msrv_mm_playerinterface/" % (ice_source_directory, _mstar_bsp, ice_source_directory),
                "cp %s/mstar/msrv_mm_playerinterface/lib/*.so %s/vendor/lib" % (ice_source_directory, project_release_dir),
                "cp -fr %s/mstar/ipc_svc/G10_NFX50/*.so %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                "echo -e \"\033[36m Building Netflix \033[0m\" ",
                "mkdir -p %s/vendor/netflix" % (project_release_dir),
                "mkdir -p %s/AppData/netflix" % (project_release_dir),
                "mkdir -p %s/mstar/netflix/tmpFolder" % (ice_source_directory),
                "tar zxvf %s/mstar/netflix/G10/Netflix5.0.3_PKG.tgz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, ice_source_directory),
                "cp -fr %s/mstar/netflix/tmpFolder/Netflix_PKG/data %s/AppData/netflix/" % (ice_source_directory, project_release_dir),
                "cp -fr %s/mstar/netflix/tmpFolder/Netflix_PKG/env %s/vendor/netflix/" % (ice_source_directory, project_release_dir),
                "rm -rf %s/mstar/netflix/tmpFolder/Netflix_PKG/arm.hard_float.debug/symbol" % (ice_source_directory),
                "cp -fr %s/mstar/netflix/tmpFolder/Netflix_PKG/arm.hard_float.debug/* %s/vendor/netflix/" % (ice_source_directory, project_release_dir),
                "cp -f %s/AppData/netflix/data/etc/conf/oem.xml %s/AppData/netflix/data/etc/conf/oem_default.xml" % (project_release_dir, project_release_dir),
                "cp -f %s/AppData/netflix/data/etc/conf/oem.xml %s/AppData/netflix/data/etc/conf/oem_nocache.xml" % (project_release_dir, project_release_dir),
                "cp -f %s/mstar/netflix/G10/NetflixInfo.ini %s/AppData/netflix/data/etc/conf/" % (ice_source_directory, project_release_dir),
                "sed -i 's/\\(<disk_cache_capacity.*>\\).*\\(<\\/disk_cache_capacity.*\\)/\\10M\\2/' %s/AppData/netflix/data/etc/conf/oem_nocache.xml" % (project_release_dir),
                "sed -i 's/\\(<media_cache_capacity.*>\\).*\\(<\\/media_cache_capacity.*\\)/\\10M\\2/' %s/AppData/netflix/data/etc/conf/oem_nocache.xml" % (project_release_dir),
                "sed -i 's/\\(<dpi_videobufferpoolsize.*>\\).*\\(<\\/dpi_videobufferpoolsize.*\\)/\\1193986560\\2/' %s/AppData/netflix/data/etc/conf/oem_default.xml" % (project_release_dir),
                "cp -f %s/AppData/netflix/data/etc/conf/oem_default.xml %s/AppData/netflix/data/etc/conf/oem_dv_default.xml" % (project_release_dir, project_release_dir),
                "cp -f %s/AppData/netflix/data/etc/conf/oem_nocache.xml %s/AppData/netflix/data/etc/conf/oem_dv_nocache.xml" % (project_release_dir, project_release_dir),
                "sed -i 's/<dpi_video_codecs>.*/<dpi_video_codecs>avc,vp9,vp9-hdr,hevc,dv5,hdr10<\\/dpi_video_codecs>/g' %s/AppData/netflix/data/etc/conf/oem_dv_nocache.xml" % (project_release_dir),
                "sed -i 's/<dpi_video_codecs>.*/<dpi_video_codecs>avc,vp9,vp9-hdr,hevc,dv5,hdr10<\\/dpi_video_codecs>/g' %s/AppData/netflix/data/etc/conf/oem_dv_default.xml" % (project_release_dir),
                # ----> glib <---- #
                "tar zxvf %s/mstar/netflix/G10/glib-2.24.2.tar.gz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, ice_source_directory),
                "cp -fa %s/mstar/netflix/tmpFolder/glib-2.24.2/lib/libglib*so* %s/vendor/netflix/lib/" % (ice_source_directory, project_release_dir),
                # ----> libpng <---- #
                "tar zxvf %s/mstar/netflix/G10/libpng-1.2.32.tar.gz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, ice_source_directory),
                "cp -fa %s/mstar/netflix/tmpFolder/libpng-1.2.32/lib/libpng*so* %s/vendor/netflix/lib/" % (ice_source_directory, project_release_dir),
                # ----> libjpeg <---- #
                "tar zxvf %s/mstar/netflix/G10/jpeg-6b.tar.gz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, ice_source_directory),
                "cp -fa %s/mstar/netflix/tmpFolder/jpeg-6b/lib/libjpeg*so* %s/vendor/netflix/lib/" % (ice_source_directory, project_release_dir),
                # ----> libfreetype <---- #
                "tar zxvf %s/mstar/netflix/G10/freetype-2.8.0.tar.gz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, ice_source_directory),
                "cp -fa %s/mstar/netflix/tmpFolder/freetype-2.8.0/libfreetype*so* %s/vendor/netflix/lib/" % (ice_source_directory, project_release_dir),
                # ----> libz <---- #
                "tar zxvf %s/mstar/netflix/G10/zlib-1.2.8.tar.gz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, ice_source_directory),
                "cp -fa %s/mstar/netflix/tmpFolder/zlib-1.2.8/lib/libz*so* %s/vendor/netflix/lib/" % (ice_source_directory, project_release_dir),
                # ----> libssl <---- #
                "tar zxvf %s/mstar/netflix/G10/openssl-0.9.8zc.tar.gz -C %s/mstar/netflix/tmpFolder/" % (ice_source_directory, ice_source_directory),
                "cp -fa %s/mstar/netflix/tmpFolder/openssl-0.9.8zc/lib/libssl*so* %s/vendor/netflix/lib/" % (ice_source_directory, project_release_dir),
                "cp -fa %s/mstar/netflix/tmpFolder/openssl-0.9.8zc/lib/libcrypto*so* %s/vendor/netflix/lib/" % (ice_source_directory, project_release_dir),
                # ----> libcares <---- #
                "if [ %d eq 1 ]; then tar zxvf %s/mstar/netflix/G10/libcares.tar.gz -C %s/mstar/netflix/tmpFolder/;fi" % (_connected, ice_source_directory, ice_source_directory),
                "if [ %d eq 1 ]; then cp -fa %s/mstar/netflix/tmpFolder/c-ares/lib/libcares*so* %s/vendor/netflix/lib/;fi" % (_connected, ice_source_directory, project_release_dir),
                # ----> misc libs <---- #
                "cp -f %s/mstar/app_ipc_interface/G3X/*.so %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                # ----> removing wrong version libs <---- #
                "rm %s/vendor/netflix/lib/libexpat.so*" % (project_release_dir),
                "rm -rf %s/vendor/netflix/lib/playready"  % (project_release_dir),
                "rm -fr %s/mstar/netflix/tmpFolder" % (ice_source_directory),
                "echo -e \"\033[36m End of building Netflix \033[0m\" ",
            )
        )
        pre_order += ' build_apm'
        apm_ld_flags = ' -L' + os.path.join(project_build_dir, 'environment/release/vendor/lib')
        libAPM += ' -l'.join([
                                    '-lAPM',
                                    'mdroid',
                                    'cusinfo',
                                    'msrv_mmplayer_ipc',
                                    'mmplayerservice',
                                    'AppIpc',
                                    'AppIpcService',
                                    'SNApi',
                                    'SNApiService',
                                    ])

    aws_iot_ld_flag = ' '
    aws_iot_dynamic_lib = ' '
    aws_iot_include = ' '
    if include_options.aws_iot:
        use_interface('3rd_party/iot/aws_iot_sdk')
        aws_iot_path = '3rd_party/iot/aws_iot_sdk'
        aws_iot_dir = 'project/aws_iot/output/mb181/openssl'
        aws_iot_include += ' {}/include'.format(aws_iot_dir)
        special_build(
            'build_aws_iot',
            command = (
                "mkdir -p %s/vendor/aws_iot_certs" % (project_release_dir),
                "cp -f %s/certs/e9c99594cd-certificate.pem %s/vendor/aws_iot_certs/b70682eddb-certificate.pem" % (aws_iot_path, project_release_dir),
                "cp -f %s/certs/rootCA.crt %s/vendor/aws_iot_certs/" % (aws_iot_path, project_release_dir),
                "cp -f %s/certs/e9c99594cd-private-enc.key %s/vendor/aws_iot_certs/b70682eddb-private-enc.key" % (aws_iot_path, project_release_dir),
                "cp -f %s/Alexa/AlexaClientSDKConfig.json %s/vendor/aws_iot_certs/" % (config_directory, project_release_dir),
                "cp -f -P {}/lib/*.so* {}/vendor/lib".format(aws_iot_dir, project_release_dir),
                "echo -e \"\033[36m End of building AWS IOT \033[0m\" ",
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )
        pre_order += ' build_aws_iot'
        aws_iot_ld_flag += ' -L' + os.path.join(project_release_dir, 'vendor/lib')
        aws_iot_dynamic_lib = ' '.join([
            '-laws_iot_mqtt',
            '-lopenssl_posix',
            '-laws_iot_backoff_algorithm',
            '-laws_iot_http',
            '-laws_iot_json',
            '-laws_iot_shadow',
            '-lplaintext_posix',
            '-lclock_posix',
            '-lsockets_posix',
        ])

    # copy nghttp2 libs if libcurl was built with nghttp2 support
    nghttp2_dynamic_libs = ''
    if include_options.nghttp2:
        special_build(
            'copy_nghttp2_lib',
            command = (
                "cp -f -P project/nghttp2/output/mb181/lib/*.so* %s/vendor/lib" % (project_release_dir),
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )
        pre_order += ' copy_nghttp2_lib'
        nghttp2_dynamic_libs +=  ' -lnghttp2 '

    playfi_dir = '3rd_party/playfi/MTK'
    playfi_libs = ''
    if include_options.play_fi:
        use_interface('3rd_party/playfi/')
        playfi_libs = ' -lPlayFiControllerSDK -l:libcrypto.so.1.1 -l:libssl.so.1.1'
        special_build(
            'copy_playfi_lib',
            command = (
                "cp -f {}/public/MB180/ControllerSDK/lib/*.so* {}/vendor/lib".format(playfi_dir, project_release_dir),
                "mkdir -p {}{}".format(project_release_dir, _playfi_ro_path),
                "mkdir {}{}/certs".format(project_release_dir, _playfi_ro_path),
                "cp -rf {}/public/MB180/OSS/ {}/{}".format(playfi_dir, project_release_dir, _playfi_ro_path),
                "cp -f -r -R -L {}/public/MB180/PLAYFI/*  {}/{}".format(playfi_dir, project_release_dir , _playfi_ro_path), #-L is for sym link problem. should be checked
                "mv {}/{}/etc/*  {}/{}/".format(project_release_dir , _playfi_ro_path, project_release_dir , _playfi_ro_path),
                "echo 5 > {}/{}/moduleVersion".format(project_release_dir, _playfi_ro_path),
                "touch {}/{}/f506738b-d58d-4849-a733-9bf31dedea49".format(project_release_dir, _playfi_ro_path),
                "touch {}/{}/cb68a726-7139-4359-a538-a15deae6df1c".format(project_release_dir, _playfi_ro_path),
                "mkdir {}/{}/playfi".format(project_release_dir, _vestel_conf_pri_rw_path),
                "mv {}/{}/allCerts.pem {}/{}/certs/".format(project_release_dir, _playfi_ro_path, project_release_dir, _playfi_ro_path ), #cert
             ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )
        pre_order += ' copy_playfi_lib'

    ffmpeg_dir = 'project/ffmpeg/output/mb181/openssl'
    avs_dir = r'project/avs/output/mb181/openssl'

    avs_ld_flag = ''
    avs_dynamic_lib = ''
    avs_static_libs = ''
    if include_options.avs:
        special_build(
            'copy_avs_lib',
            command = (
                "cp -f -P {}/lib/*.so* {}/vendor/lib".format(ffmpeg_dir, project_release_dir),
                "cp -f -P project/portaudio/output/mb181/lib/libportaudio.so* %s/vendor/lib" % (project_release_dir),
                "cp -f -P project/sqlite/output/mb181/lib/*.so* %s/vendor/lib" % (project_release_dir),
                "cp -f -P {}/lib/*.so* {}/vendor/lib".format(avs_dir, project_release_dir),
                "if [ {} -eq 1 ]; then cp -f -P project/{}/lib/*.so* {}/vendor/lib/;fi".format(_include_options_avs_mmsdk, _mmsdk_output_dir, project_release_dir),
                "mkdir -p %s/vendor/Alexa" % (project_release_dir),
                "cp -f -P %s/Alexa/*.json %s/vendor/Alexa" % (config_directory, project_release_dir),
                "mkdir -p %s/vendor/Alexa/audio" % (project_release_dir),
                "cp -f -P %s/audio/med_ui_*.wav %s/vendor/Alexa/audio" % (avs_dir, project_release_dir),
                "if [ {} -eq 1 ]; then cp -fr -P project/{}/lib/GUI {}/vendor/Alexa/;fi".format(_include_options_avs_mmsdk, _mmsdk_output_dir, project_release_dir),
                "if [ {} -eq 1 ]; then cp -fr -P project/{}/ambienthome_settings {}/vendor/Alexa/;fi".format(_avs_mmsdk_ambienthome_support, _mmsdk_output_dir, project_release_dir),
                "if [ {} -eq 1 ]; then cp -fr -P project/{}/ambienthome_demo {}/vendor/Alexa/;fi".format(_avs_mmsdk_ambienthome_demo_support, _mmsdk_output_dir, project_release_dir),
                "if [ %d -eq 1 ]; then cp -fr -P %s/Alexa/certificates %s/vendor/Alexa/;fi" % (_include_options_avs_mmsdk, config_directory, project_release_dir),
                "mkdir -p %s/vendor/Alexa/models" % (project_release_dir),
                "cp -f -P {}/models/WR_250k.en-GB.alexa.bin  {}/vendor/Alexa/models/en-GB.bin".format(avs_dir, project_release_dir),
                "cp -f -P {}/models/D.de-DE.alexa.bin  {}/vendor/Alexa/models/de-DE.bin".format(avs_dir, project_release_dir),
                "cp -f -P {}/models/D.fr-FR.alexa.bin  {}/vendor/Alexa/models/fr-FR.bin".format(avs_dir, project_release_dir),
                "cp -f -P {}/models/D.it-IT.alexa.bin  {}/vendor/Alexa/models/it-IT.bin".format(avs_dir, project_release_dir),
                "cp -f -P {}/models/D.es-ES.alexa.bin  {}/vendor/Alexa/models/es-ES.bin".format(avs_dir, project_release_dir),
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )
        pre_order += ' copy_avs_lib'
        avs_ld_flag += ' -L' + os.path.join(project_build_dir, 'environment/release/ROOTFS/lib')
        avs_dynamic_lib += ' -lstdc++ '
        avs_dynamic_lib += ' -latomic '
        avs_dynamic_lib += ' -lsqlite3 '
        avs_dynamic_lib += ' -lportaudio '
        avs_dynamic_lib += ' -lpryon_lite '
        if avs_ffmpeg_version == '4.4':
            avs_dynamic_lib += ' -l:libavcodec.so.59.1.101 '
            avs_dynamic_lib += ' -l:libavformat.so.59.2.102 '
            avs_dynamic_lib += ' -l:libavutil.so.57.0.100 '
            avs_dynamic_lib += ' -l:libswresample.so.4.0.100 '
            avs_dynamic_lib += ' -l:libavdevice.so.59.0.100 '
            avs_dynamic_lib += ' -l:libavfilter.so.8.0.101 '
            avs_dynamic_lib += ' -l:libswscale.so.6.0.100 '
        else:
            avs_dynamic_lib += ' -l:libavcodec.so.58.18.100 '
            avs_dynamic_lib += ' -l:libavformat.so.58.12.100 '
            avs_dynamic_lib += ' -l:libavutil.so.56.14.100 '
            avs_dynamic_lib += ' -l:libswresample.so.3.1.100 '
            avs_dynamic_lib += ' -l:libavdevice.so.58.3.100 '
            avs_dynamic_lib += ' -l:libavfilter.so.7.16.100 '
            avs_dynamic_lib += ' -l:libswscale.so.5.1.100 '
        avs_dynamic_lib += avs_device_sdk_dynamic_libs
        avs_static_libs += avs_device_sdk_static_libs
        if include_options.avs_mmsdk:
            avs_dynamic_lib +=  alexa_smart_screen_sdk_dynamic_libs
            avs_static_libs += alexa_smart_screen_sdk_static_libs
        else:
            avs_dynamic_lib +=  ' -lDefaultClient '

    #if 'nfs' not in options.targets and 'nfslite' not in options.targets:
    #    special_build(
    #        'partition_all',
    #        command = (
    #            "cd %s/environment && make partition-all" % project_build_dir,
    #        ),
    #        required_by = cbuild_first,
    #        requires = pre_order,
    #        phony = True,
    #    )
    #    pre_order += ' partition_all'

    ###############################################################################

    if not mstar_user_build and not vewd_user_build and not amazon_user_build:
        if (include_options.dlna
            and 'nothing' not in options.targets
            and include_options.nflc30):
            if not include_options.follow_tv:
                dlna_target_conf = 'DMP-DMR'
            else:
                dlna_target_conf = 'ALL'
            if options.clean_all:
                special_build(
                    'clean_dlna_libs',
                    command = (
                        "export PATH=${PATH}:%s/bin; cd %s/../libdlna_access_3.0/dlna/target/linux; ./clean_dlna_libs_mb181.sh" % (toolchain_dir, project_directory),
                    ),
                    required_by = cbuild_first,
                    requires = pre_order,
                    phony = True,
                )
                pre_order += ' clean_dlna_libs'
            special_build(
                'build_dlna_libs',
                command = (
                    "export PATH=${PATH}:%s/bin; export BUILD_TARGET=%s; cd %s/../libdlna_access_3.0/dlna/target/linux; ./build_dlna_libs_mb181.sh" % (toolchain_dir, dlna_target_conf, project_directory),
                ),
                required_by = cbuild_first,
                requires = pre_order,
                phony = True,
            )
            pre_order += ' build_dlna_libs'
               

    ###############################################################################
    if include_options.vewd_browser:
       use_interface('3rd_party/libmpg123')

    if _include_xpca:
        use_interface('3rd_party/xpca_v2.0')
        use_interface('3rd_party/xpca_v2.0/include')
        use_interface('3rd_party/xpca_v2.0/xpca_lib/mstar')

    ###############################################################################
    _netfront_flags = ''

    if (include_options.netfront41 or include_options.netfront42) and ('D' in canonical_variant or include_options.slim_debug):
        _netfront_flags += ' _DEBUG'


    libxml2_include = ''
    if include_options.fvc or include_options.dvbi:
        libxml2_include = 'project/libxml2/output/mb181/include/libxml2'

    gles_include = ''
    if include_options.use_gles or include_options.gles_ui:
        gles_include = '%s/mstar/dfb/directfb-1.4.2m/include/directfb %s/mstar/mali/include'  % (ice_source_directory, ice_source_directory)

    openssl_dir = 'project/openssl/output/mb181'
    libcurl_dir = 'project/libcurl/output/mb181/openssl'

    openssl_include = '{}/include'.format(openssl_dir)
    libcurl_include = '{}/include'.format(libcurl_dir)
    libpng_include = 'project/libpng/output/mb181/include'
    zlib_include = 'project/zlib/output/mb181/include'
    freetype_include = '%s/project/freetype/output/mb181/include/freetype2/' % project_root

    _depot_third_party_headers = '{} {}'.format(openssl_include, libcurl_include)

    _project_includes = ' '.join([openssl_include, libcurl_include, libxml2_include, 
                                  gles_include, freetype_include, libpng_include, 
                                  zlib_include, aws_iot_include])
    if include_options.play_fi:
        _project_includes += ' {}/private/mb180/ControllerSDK/include'.format(playfi_dir)
    if include_options.avs:
        _project_includes += ' {}/include'.format(avs_dir)
        _project_includes += ' project/portaudio/output/mb181/include'
        if _avs_caption_support:
            _project_includes += ' project/webvtt/output/mb181/include'

    if include_options.sqlite:
        _project_includes += ' project/sqlite/output/mb181/include'

    if include_options.ffmpeg:
        _project_includes += ' {}/include'.format(ffmpeg_dir)

    if include_options.avs_mmsdk:
        _project_includes += ' project/{}/include'.format(_mmsdk_output_dir)
        _project_includes += ' project/asio/output/mb181/include'
        _project_includes += ' project/websocketpp/output/mb181/include'
        _project_includes += ' project/apl/output/mb181/include'

    _project_defines = 'CURL_STATICLIB' + _netfront_flags
    _include_hdmi_enchanced_color = None
    
    if include_options.g32:
        _include_hdmi_enchanced_color = 1
    
    if include_options.g31:
        _include_hdmi_enchanced_color = 1

    if include_options.iptuner or include_options.follow_tv_rtsp:
        _project_includes = _project_includes + ' project/live555/output/mb181/include'

    if include_options.connected:
        if include_options.g31:
            _subtitle_delay_with_movie_sense = 50
            _subtitle_delay_without_movie_sense = 50
        else:
            _subtitle_delay_with_movie_sense = 10
            _subtitle_delay_without_movie_sense = 10
    else:
        _subtitle_delay_with_movie_sense = 270
        _subtitle_delay_without_movie_sense = 270

    pubsub_include = ''
    if _include_extended_logging:
        pubsub_3rd_party_include = '%s/3rd_party/include' % pubsub_tmp_directory
        pubsub_include = pubsub_tmp_directory + ' ' + pubsub_3rd_party_include
        _project_includes = _project_includes + ' ' + pubsub_include

    if include_options.vewd_sdk423:
        rpclib_include = 'project/rpclib/output/mb181/include'
        _project_includes = _project_includes + ' ' + rpclib_include

    libwebsockets_dir = ''
    libwebsockets_libs = ''
    libwebsockets_ld_flags = ''
    if include_options.websocket:
        libwebsockets_dir = 'project/libwebsockets/output/mb181/openssl'

        special_build(
            'copy_remote_diagnostic_certs',
            command = (
                "cp -rf {}/../vendor/Vestel/public/libwebsockets/remote_diagnostic_certs {}/vendor/remote_diagnostic_certs".format(project_directory, project_release_dir),
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )

        pre_order += ' copy_remote_diagnostic_certs'

        _project_includes += ' {}/include'.format(libwebsockets_dir)
        libwebsockets_ld_flags = '-L{}/lib'.format(libwebsockets_dir)
        libwebsockets_libs = '-lwebsockets'

    set_shared(
        'project_shared',
         DEPOT_THIRD_PARTY_HEADERS = _depot_third_party_headers,
         PROJECT_INCLUDES = _project_includes,
         PROJECT_DEFINES= _project_defines,
         INCLUDE_HDMI_ENHANCED_COLOR = _include_hdmi_enchanced_color,
         SUBTITLE_DELAY_WITH_MOVIE_SENSE = _subtitle_delay_with_movie_sense,
         SUBTITLE_DELAY_WITHOUT_MOVIE_SENSE = _subtitle_delay_without_movie_sense
        )

    if not include_options.ice_tests and not include_options.challenger:
        use_component('3rd_party/jsoncpp')
        use_component('3rd_party/libjsmn')

    ###############################################################################

    ###############################################################################
    all_static_libs = '$(LIBS)'
    openssl_libs = '-lssl -lcrypto'
    png_libs = '-lpng'
    freetype_libs = '-lfreetype'
    zlib_libs = '-lz'
    dlna_libs = ''
    curl_platform_libs = ''
    libwifi=''
    discretix_libs=''
    vmx_web_libs=''
    utopia_static_libs = ''
    libbsa = ''
    libxml2_lib = ''
    rpclib_lib = 'project/rpclib/output/mb181/lib/librpc.a'
    rpclib_nopic_lib = 'project/rpclib/output/mb181/nopic/lib/librpc.a'
    if (include_options.fvc or include_options.dvbi) and not mstar_user_build:
        libxml2_lib = 'project/libxml2/output/mb181/lib/libxml2.a'

    xpca_libs = ''
    xpca_ld_flags = ''
    if _include_xpca:
        xpca_ld_flags = '-L' + os.path.join(project_build_dir, 'environment/release/vendor/lib') 
        xpca_libs += '-lWasabi -lSkb'

    all_dynamic_libs = ''
    all_ld_flags = ''
    utopia_dynamic_libs = ''
    utopia_ld_flags = ''
    openssl_ld_flags = '-L' + os.path.join(project_root, '{}/lib'.format(openssl_dir))
    png_ld_flags = '-L' + os.path.join(project_root, 'project/libpng/output/mb181/lib')
    freetype_ld_flags = '-L' + os.path.join(project_root, 'project/freetype/output/mb181/lib')
    liblive555 = ''
    quickshare = ''

    optee_libs = ' -l'.join([
                                '-lintertaca',
                                #'xtest',
                                'teec',
                                'DDI_SMP',
                                'hdcp',
                                'hdcpCA',
                                'intertaca',
                                'protobuf',
                                'oemcryptoCENC_v16',
                                'widevine_ce_cdm_shared',
                                'playready4.x_tee',
                                'Wasabi',
                                'Skb'
                ])

    ###############################################################################
    if include_options.ice_tests:
        use_component('main_icetests_generic')
        use_component( 'icetests',
            MSTAR_DIAMOND = 1)
    elif include_options.challenger:
        use_component('main_challenger_with_profile_manager')
        use_component('challenger',
            INCLUDE_UI_MESSAGE = 1,
            PLATFORM_STRING = '"Aurora"',
            MSTAR_DIAMOND = 1)

    if vewd_user_build or mstar_user_build or amazon_user_build:
        if include_options.discretix_drm:
                use_interface('3rd_party/discretix_drm')

    if not mstar_user_build and not vewd_user_build and not amazon_user_build:
        if not include_options.ice_tests and not include_options.challenger:
            if include_options.freespace:
                use_external_component('freespace')
            if 1:
                use_component('main_eclipse_with_profile_manager')
            else:
                use_component('main_eclipse_generic')

        if not include_options.ice_tests and not include_options.challenger:
            if include_options.netfront41:
                use_component('AccessNFB_4.1',
                               ICE_OSD_ON_NEXUS = not include_options.use_dfb,
                               BUILD_FOR_MSTAR =_mstar_platform)
            elif include_options.netfront42 :
                use_component('AccessNFB_4.2',
                               ICE_OSD_ON_NEXUS = not include_options.use_dfb)

            if include_options.discretix_drm:
                use_interface('3rd_party/discretix_drm')

            if include_options.vmx_web:
                use_interface('3rd_party/verimatrix_web_3.7')

        if (include_options.dlna
            and include_options.nflc30
            and include_options.nflc):
            use_libdlna_access_3_0 = True
            use_component('libdlna_access_3.0')
            if include_options.follow_tv:
                dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libdms.a '
                dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libdmc.a '
                dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libupnp_server.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libdmr.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libdmp.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/ext/exif.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/ext/libinet.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/ext/libexpat.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libnflcclient.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libplusup.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libplusdown.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libdlnacommon.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libupnp_client.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libupnp_server_newdmr.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libdlnamedia.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/libdlnautil.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/peer/libpeer.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/ext/libslim.a '
            dlna_libs += 'libdlna_access_3.0/dlna/target/linux/libs/ext/libslim_peer.a '

    if not mstar_user_build:
        curl_platform_libs += '{}/lib/libcurl.a '.format(libcurl_dir)
        curl_platform_libs += 'project/c-ares/output/mb181/lib/libcares.a '

    special_build(
        'copy_dependencies',
        command = (
            'cp -a {}/*.so* {};'.format(os.path.join(project_root, '{}/lib'.format(openssl_dir)),
                                        os.path.join(project_release_dir, "vendor/lib")),
            'cp -a {}/*.so* {};'.format(os.path.join(project_root, 'project/libpng/output/mb181/lib'),
                                        os.path.join(project_release_dir, "vendor/lib")),
            'cp -a {}/*.so* {};'.format(os.path.join(project_root, 'project/zlib/output/mb181/lib'),
                                        os.path.join(project_release_dir, "vendor/lib")),
            'cp -a {}/*.so* {};'.format(os.path.join(project_root, '{}/lib'.format(libwebsockets_dir)),
                                        os.path.join(project_release_dir, "vendor/lib")),
            'cp -a {}/*.so* {};'.format(os.path.join(project_root, 'project/libxml2/output/mb181/lib'),
                                        os.path.join(project_release_dir, "vendor/lib")),
            'cp -a {}/*.so* {};'.format(os.path.join(project_root, 'project/freetype/output/mb181/lib'),
                                        os.path.join(project_release_dir, "vendor/lib")),
        ),
        required_by = 'elf',
        phony = True
        )

    ###############################################################################
    if (include_options.iptuner or include_options.follow_tv_rtsp):
        output_lib_dir = os.path.join(project_root, "project/live555/output/mb181/lib/")
        output_vendor_dir = os.path.join(project_release_dir, "vendor/lib/")

        special_build(
            'setup_live555',
            command = (
                "mkdir -p %s"% (output_vendor_dir),
                "cp -f %slibliveMedia.so %s" % (output_lib_dir, output_vendor_dir),
                "cp -f %slibgroupsock.so %s" % (output_lib_dir, output_vendor_dir),
                "cp -f %slibBasicUsageEnvironment.so %s" % (output_lib_dir, output_vendor_dir),
                "cp -f %slibUsageEnvironment.so %s" % (output_lib_dir, output_vendor_dir)
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )
        pre_order += ' setup_live555'

        liblive555 = ' '.join([
                "%slibliveMedia.so" % (output_lib_dir),
                "%slibgroupsock.so" % (output_lib_dir),
                "%slibBasicUsageEnvironment.so" % (output_lib_dir),
                "%slibUsageEnvironment.so" % (output_lib_dir),
                ])

    ###############################################################################
    
 
    if (include_options.iptuner or include_options.follow_tv_rtsp):
        quickshare_lib_dir = os.path.join(project_root, "3rd_party/quickshare/")
        quickshare_vendor_dir = os.path.join(project_release_dir, "vendor/lib/")  #  Where libraries are staged        
        use_interface('3rd_party/quickshare')    
        special_build(
            'quickshare_lib',  #  Give your build step a descriptive name
            command=(
                "cp -f %slibairplayer.so %s" % (quickshare_lib_dir, quickshare_vendor_dir),
                "cp -f %slibairplay_sdk.so %s" % (quickshare_lib_dir, quickshare_vendor_dir),
                "chmod +x %s/libairplay_sdk.so %s/libairplayer.so" % (quickshare_vendor_dir, quickshare_vendor_dir),
                "echo -e '\\033[36m Copied libairplay_sdk.so \\033[0m'",  #  Optional:  A message to show during the build
            ),
            required_by=cbuild_first,  #  Or another appropriate build step
            requires = pre_order,
            phony=True,
        )
        pre_order += ' quickshare_lib'
        
        quickshare = '-L%s -lairplay_sdk -lairplayer' % quickshare_vendor_dir
    
    ###############################################################################

    use_external_component(config_directory)

    if include_options.discretix_drm:
        if  'D' in canonical_variant:
            build_variant = 'debug'
        else:
            build_variant = 'release'

        discretix_release_dir = '3rd_party/discretix_drm/mb130/dx_linux_v1.0.0.98'
        discretix_ld_flag += ' -L' + os.path.join(project_build_dir, 'environment/release/vendor/browser_dir/uva_drm_backends')

        special_build(
            'setup_discretix',
            command = (
                "mkdir -p %s/vendor/browser_dir/uva_drm_backends" % (project_release_dir) ,
                "cp -f %s/discretix_library/%s/libuvadrmbackend-playready.so %s/vendor/browser_dir/uva_drm_backends" % (discretix_release_dir, build_variant, project_release_dir) ,
                "cp -f %s/discretix_library/%s/libDxBridge.so %s/vendor/lib" % (discretix_release_dir, build_variant, project_release_dir) ,
                "cp -f %s/discretix_library/%s/libTrusted.so %s/vendor/lib" % (discretix_release_dir, build_variant, project_release_dir) ,
                "cp -f %s/discretix_library/%s/libDxDrm.so %s/vendor/lib" % (discretix_release_dir, build_variant, project_release_dir),
                "mkdir -p %s/vendor/drm_data" % (project_release_dir) ,
                "mkdir -p %s/vendor/drm_data/revocation_files" % (project_release_dir) ,
                "mkdir -p %s/vendor/drm_data/revocation_files/PlayReady" % (project_release_dir) ,
                "mkdir -p %s/conf/discretix_drm_data" % (project_release_dir) ,
                "mkdir -p %s/conf/discretix_drm_data/log_files" % (project_release_dir) ,
                "mkdir -p %s/conf/discretix_drm_data/secure_database" % (project_release_dir) ,
                "mkdir -p %s/conf/discretix_drm_data/storage" % (project_release_dir),
                "cp -rf %s/drm_data/revocation_files/PlayReady/*  %s/vendor/drm_data/revocation_files/PlayReady/" % (config_directory, project_release_dir)
            ),
            required_by = cbuild_first,
            requires = pre_order + ' release_lib_package',
            phony = True,
        )
        pre_order += ' setup_discretix'

        discretix_libs += '-lDxDrm '
        discretix_libs += '-lTrusted '
        discretix_libs += '-luvadrmbackend-playready '
        discretix_libs += '-lDxBridge '

    if _build_drm_backend or _build_ciplus_drm_backend:
        vmx_web_lib_release = 'VRWebClient_STD_3.7.0.0_CI7_vestel-mb100_PROD_2.0.0'
        vmx_web_lib_release_dir = "3rd_party/verimatrix_web_3.7/%s" % (vmx_web_lib_release)
        special_build(
            'build_drm_backend',
            command = (
                "mkdir -p %s/vendor/browser_dir/uva_drm_backends" % (project_release_dir),
                "export PATH=${PATH}:%s/bin; %s/scripts/build-vewd-drmbackends.sh %s %s %s %d %s %d %s %d %d %d %s %d" %
                    (toolchain_dir, project_build_dir, project_root, vewd_sdk_dist_dir, vewd_module,
                    _include_widevine_support and _build_drm_backend, widevine_release_dir,
                    _include_verimatrix_support and _build_drm_backend, vmx_web_lib_release_dir,
                        'A' in canonical_variant,
                        'T' in canonical_variant,
                        'V' in canonical_variant,
                        '$(derived_dir)',
                        1 if 'build_dynamic_ui' not in options.targets else 0),
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )

    if _include_widevine_support:
        special_build(
            'build_widevine',
            command = (
                "echo '********************* Copy Widevine Shared lib ************************'",
                "mkdir -p %s/vendor/browser_dir/uva_drm_backends/widevine_cdm" % (project_release_dir),
                "cp -f %s/3rdpartylibs/widevine/libwidevine_ce_cdm_shared.so %s/vendor/browser_dir/uva_drm_backends/widevine_cdm"  % (vewd_sdk_dist_dir, project_release_dir),
                ),
        requires = 'copy_vewd_dirs',
        phony = True,
        )
        pre_order += ' build_widevine'

    if _build_drm_backend and _include_verimatrix_support:
        special_build(
            'build_vmx_web',
            command = (
                "echo '********************* Copy Verimatrix Web Shared lib ************************'",
                "cp -f %s/lib/libvrwebclient.so %s/vendor/lib"  % (vmx_web_lib_release_dir, project_release_dir),
                "cp -f %s/libuvadrmbackend-viewright.so %s/vendor/browser_dir/uva_drm_backends"  % (vewd_drm_backend_dir, project_release_dir),
                ),
        requires = 'build_drm_backend',
        phony = True,
        )
        pre_order += ' build_vmx_web'

    if _build_ciplus_drm_backend:
        special_build(
            'copy_oipfbackend',
            command = (
                "echo '********************* Copy CI+ OIPF Backend Shared lib ************************'",
                "cp -f $(derived_dir)/%s/drm_backends/ciplus/libuvadrmbackend-ciplus.so %s/vendor/browser_dir/uva_drm_backends/"  % (vewd_module, project_release_dir),
                ),
        requires = 'build_drm_backend',
        phony = True,
        )
        pre_order += ' copy_oipfbackend'

    if not include_options.ice_tests and not include_options.challenger:
        if (include_options.hbbtv or include_options.portal or include_options.iplayer):
            if (not include_options.netfront41) and (not include_options.netfront42) and (not include_options.vewd_browser):
                warning("Warning: HbbTV, Portal or iPlayer is specified but no browser is being build")
            else:
                if include_options.netfront41 or include_options.netfront42:
                    use_interface('netfront',
                                  FREETYPE_USES_CABOT_MEMORY_MANAGER = None,
                                  BROWSER_EXTRA_FONTS = None,
                                  IPLAYER_CERT_FILE_PATH = '"/conf/browser/iplayer_client_cert/iplayer_client.inf"',
                                  STARTURL_FILE_PATH= _starturl_file_path)
        if _include_jpeg_support:
            use_component('3rd_party/jpeg',
            CABOT_JPEG_PREFIX = 1)

    if include_options.samba_acr:
        use_interface('3rd_party/samba')
        special_build(
            'copy_samba_acr',
            command = (
                "cp -f -R 3rd_party/samba/MTK/G32 %s/vendor/applications/samba" % (project_release_dir),
                "cp -f -R %s/df_gopc.gopc %s/vendor/applications/samba/samba_df_gopc.gopc" % (config_directory,project_release_dir),
                "cp -f  %s/mstar/samba/libsamba_acr.so %s/vendor/lib/" % (ice_source_directory,project_release_dir),
                "mkdir %s/conf/samba" % (project_release_dir),
            ),
        required_by = cbuild_first,
        requires = pre_order,
        phony = True,
        )
        pre_order += ' copy_samba_acr'

    if include_options.bluetooth:
        set_component_config('eclipse_src',
                             BT_PAIRED_DEVICE_DB_FILE = '"/conf/BTPairedDevicesDB.xml"',
                             BT_BLUETOOTH_HHDB_FILE = '"/conf/BluetoothHHDB.xml"')

#        special_build(
#            'setup_bluetooth',
#            command = (
#                "cp -f %s/bluetooth/brcm/arm_4_8_3/libs/libbsa.so %s/vendor/lib" % (ice_source_directory, project_release_dir),
#            ),
#            required_by = cbuild_first,
#            requires = pre_order,
#            phony = True,
#        )
#        pre_order += ' setup_bluetooth'

        libbsa = ''
    else:
        libbsa = ''

    if include_options.ice_streamer:
        use_component('ice_streamer',
                COMPILE_AS_SHARED = None,
                ICE_SOURCE_DIR = ice_source_directory)

    if include_options.tee:
        special_build(
            'copy_secure_daemon',
            command = (
                #"cp -f %s/mstar/tee/secure_daemon %s/vendor" % (ice_source_directory, project_release_dir),
            ),
            required_by = cbuild_first,
            requires = pre_order + ' release_lib_package',
            phony = True,
        )

    if include_options.dial:
        if not _use_dial_200:
            use_component(
                'dial',
                INCLUDE_DIAL_VERSION_164 = _use_dial_164,
                INCLUDE_DIAL_VERSION_170 = _use_dial_170
                )
            special_build(
                'copy_dial_files',
                command = (
                    '$(call copyfile, %s/dial/dial_appname_to_prefixurl_list.xml , %s/conf)' % (config_directory, project_release_dir)
                ),
                requires = 'vendor',
            )
        else:
            use_interface('3rd_party/dial-2.2')
            special_build(
                'copy_dial_files',
                command = (
                    #"cp -f %s/../3rd_party/dial-2.1/Mstar-turnkey/server/release_pkg_dir/dial_v2.1.x/bin/dialserver %s/vendor/dialserver" % (project_directory, project_release_dir),
                    "cp -f %s/../3rd_party/dial-2.2/Mstar-turnkey/build/dialserver %s%s/dialserver" % (project_directory, project_release_dir, _dial_ro_path),
                    "cp -f %s/../3rd_party/dial-2.2/Mstar-turnkey/porting-layer/libdial_porting_interface.so %s/vendor/lib/libdial_porting_interface.so" % (project_directory, project_release_dir),
                    "cp -f %s/../3rd_party/dial-2.2/Mstar-turnkey/porting-layer/dialappsetting.ini %s%s/dialappsetting.ini" % (project_directory, project_release_dir, _dial_ro_path)
                    ),
                #requires = 'vendor build_dial_libs',
                requires = 'vendor',
            )

        special_build(
            'copy_keypress_codes',
            command = (
                "cp -f %s/dial/ice_keymapping_db.xml %s/vendor/ice_keymapping_db.xml" % (config_directory, project_release_dir),
                "cp -f %s/dial/vk_keymapping_db.xml %s/vendor/vk_keymapping_db.xml" % (config_directory, project_release_dir),
            ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
        )

        pre_order += ' copy_keypress_codes'

    if not vewd_user_build:
        use_component(
            ice_source_directory,
            OSD_COLOR_MODEL = 2,
            OSD_RESOLUTION = _osd_resolution,
            INCLUDE_VBI_LOG = include_options.vbilog,
            ICE_SOFTWARE_VERSION = _software_version_hex,
            DCF_NAME = '"' + _full_dcf_filename + '"',
            INCLUDE_NOMPEG4 = not include_options.mpeg4,
            INCLUDE_FREESPACE = include_options.freespace,
            INCLUDE_MSTAR_COMPILE = not mstar_user_build,
            ONE_DDR_H264 = None,
            INCLUDE_FAST_TURN_ON = include_options.fastturnon,
            INCLUDE_NO_TUNER = None,
            INCLUDE_SMOOTH_STREAMING = include_options.smooth_streaming,
            USE_CAM_EMULATOR = include_options.camemulator,
            ENABLE_MSTAR_DYNAMIC = include_options.dynamic_lib,
            INCLUDE_ENABLE_DFB = include_options.use_dfb,
            INCLUDE_SECBOOT = include_options.secboot,
            INCLUDE_SEC = include_options.sec,
            INCLUDE_SUPER_RESOLUTION = include_options.super_resolution,
            INCLUDE_VIRTUAL_MOUSE = include_options.virtual_mouse,
            INCLUDE_ENABLE_NFSBUILD = _enable_nfsbuild,
            INCLUDE_ENABLE_G10 = 1,
            INCLUDE_ENABLE_G36 = include_options.g36,
            INCLUDE_ENABLE_G32 = include_options.g32,
            INCLUDE_ENABLE_G31 = include_options.g31,
            DISCRETIX_RELEASE_DIR = discretix_release_dir,
            ICE_PRINCIPAL_IDENTIFIER = '"' + _receiver_identifier_str + _chil_receiver_ident +'"',
            INCLUDE_STORAGE_IS_EMMC = include_options.emmc,
            INCLUDE_ENABLE_TEE = include_options.tee,
            INCLUDE_ENABLE_CONNECTED = include_options.connected,
            INCLUDE_ENABLE_AMAZON_PV= include_options.aprime,
        )
    else:
        use_external(
            ice_source_directory,
            DISCRETIX_RELEASE_DIR = discretix_release_dir,
        )

    ###############################################################################
    widi_link_flag = ''
    if _include_widi_support:
        widi_link_flag += ' -l'.join([
                               '-lmct',
                               'mrec',
                               'wifip2p',
                               'miracastrx'
                               ])

    _include_wifi = 0
    _include_bt = 0
    if include_options.wifi:
        libwifi = ice_source_directory + '/wifi/arm_lib/arm_4_8_3_G10/libbwl.a'
        _include_wifi = 1

    if include_options.bluetooth:
        _include_bt = 1

    if 'dynamic_lib' in conf_options and include_options.connected:
        # Make sure files are not copied if they're the same, to avoid ICE source
        # files being rebuilt on every build.
        common_cp_options = '-fu'
        special_build(
            'release_lib_package',
            command = (
                            "if [ ! -d %s%s ];then mkdir -p %s%s;fi" % (project_release_dir, _teerfs_path, project_release_dir, _teerfs_path),
                            "tar -xzf %s/mstar/ddi_smp/%s/libDDI_SMP-optee.tar.gz -C %s/mstar/ddi_smp/" % (ice_source_directory, _mstar_bsp, ice_source_directory),
                            "cp -f %s/mstar/ddi_smp/libDDI_SMP/lib/optee/* %s/vendor/lib" % (ice_source_directory, project_release_dir),              
                            "tar -xzf %s/mstar/tee/OPTEE_%s_%s.tar.gz -C %s/mstar/tee/" % (ice_source_directory, _mstar_chip, _mmap_config, ice_source_directory),
                            "cp -f %s/mstar/tee/bin/* %s/vendor/applications" % (ice_source_directory, project_release_dir),
                            "cp -rf %s/mstar/tee/config/* %s/vendor/config/" % (ice_source_directory, project_release_dir),
                            "mkdir -p %s/vendor/config/appipc" % (project_release_dir),
                            "cp -f %s/mstar/app_ipc_interface/ModelInfo.ini %s/conf/" % (ice_source_directory, project_release_dir),
                            "ln -sT /conf/ModelInfo.ini %s/vendor/config/appipc/ModelInfo.ini" % (project_release_dir),
                            "mkdir -p %s/tee" % (project_release_dir),
                            "cp -f %s/mstar/tee/image/* %s/tee" % (ice_source_directory, project_release_dir),
                            "cp -af %s/mstar/tee/lib/* %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                            #"rm -rf %s/vendor/lib/libtee_mm.so" % (project_release_dir),
                            #------ Temp solution for prelink issue, this will be fixed in optee release flow. ------ START
                            "rm -rf %s/vendor/lib/libteec.so" % (project_release_dir),
                            "rm -rf %s/vendor/lib/libteec.so.1" % (project_release_dir),
                            "ln -sT ./libteec.so.1.0 %s/vendor/lib/libteec.so.1" % (project_release_dir),
                            "ln -sT ./libteec.so.1 %s/vendor/lib/libteec.so" % (project_release_dir),
                            #------ Temp solution for prelink issue, this will be fixed in optee release flow. ------ END
                            "mv -f %s/mstar/tee/optee_armtz %s/ROOTFS/lib/" % (ice_source_directory, project_release_dir),
                            "mkdir -p %s/conf/optee_armtz" % (project_release_dir),
                            "mv -f %s/ROOTFS/lib/optee_armtz/fc5196b1-555b-4288-8f46e78472e1f05e.ta %s/" % (project_release_dir, project_release_dir),
                            "mv -f %s/ROOTFS/lib/optee_armtz/bc2f95bc-1111-2222-a43ca1796e7cac31.ta %s/conf/optee_armtz/" % (project_release_dir, project_release_dir),
                            "ln -sT /conf/optee_armtz/fc5196b1-555b-4288-8f46e78472e1f05e.ta %s/ROOTFS/lib/optee_armtz/fc5196b1-555b-4288-8f46e78472e1f05e.ta" % (project_release_dir),
                            "ln -sT /conf/optee_armtz/bc2f95bc-1111-2222-a43ca1796e7cac31.ta %s/ROOTFS/lib/optee_armtz/bc2f95bc-1111-2222-a43ca1796e7cac31.ta" % (project_release_dir),
                            #"cp -f %s/mstar/tee/debug/bin/* %s/vendor/applications" % (ice_source_directory, project_release_dir),
                            #"cp -f %s/mstar/tee/debug/ta/* %s/ROOTFS/lib/optee_armtz/" % (ice_source_directory, project_release_dir),
                            #"cp -f %s/mstar/tee/xtestsupport/libxtestsupport2.so %s/ROOTFS/lib/" % (ice_source_directory, project_release_dir),
                            "cp -f %s/mstar/tee/CusSignedEncryptedTA/*.ta %s/ROOTFS/lib/optee_armtz/" % (ice_source_directory, project_release_dir),
                            #"mv -f %s/ROOTFS/lib/optee_armtz/b9ac68a4-196f-11e9-aa163ca82a1f90b8.ta %s/conf/optee_armtz/" % (project_release_dir, project_release_dir),
                            #"ln -sT /conf/optee_armtz/b9ac68a4-196f-11e9-aa163ca82a1f90b8.ta %s/ROOTFS/lib/optee_armtz/b9ac68a4-196f-11e9-aa163ca82a1f90b8.ta" % (project_release_dir),
                            #------ DEBUG, use tee_debug  bin, config, lib, optee_armtz ---- START
                            #"cp -f %s/mstar/tee/tee_debug/bin/* %s/vendor/applications" % (ice_source_directory, project_release_dir),
                            #"cp -rf %s/mstar/tee/tee_debug/config/* %s/vendor/config/" % (ice_source_directory, project_release_dir),
                            #"cp -af %s/mstar/tee/tee_debug/lib/* %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                            #"cp -af %s/mstar/tee/tee_debug/optee_armtz/* %s/ROOTFS/lib/optee_armtz/" % (ice_source_directory, project_release_dir),
                            #------ DEBUG, use tee_debug  bin, config, lib, optee_armtz ---- END
                            #------ DEBUG, Marlin debug, move ta to /conf/optee_armtz/758ee8b4-f485-45fb-8fb03d0cc027b84a.ta ---- START
                            #"mv -f %s/ROOTFS/lib/optee_armtz/758ee8b4-f485-45fb-8fb03d0cc027b84a.ta %s/conf/optee_armtz/" % (project_release_dir, project_release_dir),
                            #"ln -sT /conf/optee_armtz/758ee8b4-f485-45fb-8fb03d0cc027b84a.ta %s/ROOTFS/lib/optee_armtz/758ee8b4-f485-45fb-8fb03d0cc027b84a.ta" % (project_release_dir),
                            #------ DEBUG, Marlin debug, move ta to /conf/optee_armtz/758ee8b4-f485-45fb-8fb03d0cc027b84a.ta ---- END
                            "rm -rf %s/mstar/tee/bin" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/config" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/image" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/lib" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/CusConfig" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/debug" % (ice_source_directory),
                            "touch %s/conf/app_pri/vestel/selinux.en" % (project_release_dir),
                            'if [ %d -eq 1 ]; then $(call copyfile, %s/mstar/widi/*Adapter.config , %s/conf);fi' % (_include_widi_support,ice_source_directory,project_release_dir),
                            'if [ %d -eq 1 ]; then $(call copyfile, %s/mstar/widi/*udhcpd_srv.config , %s%s);fi' % (_include_widi_support,ice_source_directory,project_release_dir, _udhcp_conf_pub_rw_path),
                            #'if [ %d -eq 1 ]; then $(call copyfile, %s/mstar/widi/sample_rtsp_sink , %s/conf);fi' % ( _include_widi_support,ice_source_directory,project_release_dir),
                            #'if [ %d -eq 1 ]; then $(call copyfile, %s/mstar/widi/edid.edid , %s/conf);fi' % ( _include_widi_support,ice_source_directory,project_release_dir),
                            #'strings -a %s/mstar/bsp/%s/so_lib/libapiVDEC.so | grep "VESTEL" > %s/mstar/bsp/%s/so_lib/normal_vdec_version.txt' % (ice_source_directory, _mstar_bsp, ice_source_directory, _mstar_bsp),
                            #'if ! diff -q %s/mstar/tee/TEE_Release_MB130/secure_vdec_version.txt %s/mstar/bsp/%s/so_lib/normal_vdec_version.txt ; then exit 1; fi' % (ice_source_directory, ice_source_directory, _mstar_bsp),
                            "cd %s/mstar/customerinfo/;./genCusInfo %s %s;cd -"  % (ice_source_directory, _mstar_cusInfo, toolchain_dir),
                            "cp -r %s %s/mstar/customerinfo/libcusinfo.so %s/environment/release/vendor/lib"  % (common_cp_options, ice_source_directory, project_build_dir),
                            "sh %s/mstar/bsp/%s/bin/shmver_test.sh %s/mstar/bsp/%s/bin %s/mstar/mmsdk/mmsdk/%s" % (ice_source_directory, _mstar_bsp, ice_source_directory, _mstar_bsp, ice_source_directory, _mstar_bsp),
                            "cd %s/mstar/a2dp_so/; ./genlib.sh %s; cd -;" % (ice_source_directory, toolchain_dir),
                            "cp -rf %s/mstar/a2dp_so/bt_a2dp_alsa_playback.so %s/environment/release/vendor/lib;" % (ice_source_directory, project_build_dir),
                            "cp -rf %s/mstar/a2dp_so/playback.ini %s/vendor/config;" % (ice_source_directory, project_release_dir),
                            'if [ %d -eq 1 ]; then cd %s/mstar/platform/; ./genlib.sh %s; cd -; fi' % (_include_AMZ_support, ice_source_directory, toolchain_dir),
                            'if [ %d -eq 1 ]; then cp -rf %s/mstar/platform/libplatform.so %s/environment/release/vendor/lib; fi' % (_include_AMZ_support, ice_source_directory, project_build_dir),
            ),
            required_by = cbuild_first,
        )
        pre_order += ' release_lib_package'
        utopia_ld_flags = ' -L' + os.path.join(project_build_dir, 'environment/release/vendor/lib')
        if include_options.aprime:
            utopia_dynamic_libs += ' -l'.join([
                                        '-lapiACE',
                                        'apiAUDIO',
                                        'apiCEC',
                                        'apiDLC',
                                        'apiDMX',
                                        'apiGFX',
                                        'apiGOP',
                                        'apiGPD',
                                        'apiJPEG',
                                        'apiMFE',
                                        'apiPNL',
                                        'apiSWI2C',
                                        'apiVDEC',
                                        #'apiVDEC_V1',
                                        'apiXC',
                                        'hdcp1x',
                                        'demoCA',
                                        'pvr_CA',
                                        'hdcp1xCA',
                                        'drvPQ',
                                        'drvAESDMA',
                                        #'drvAUDIO',
                                        'drvAUDSP',
                                        'drvAVD',
                                        'drvCPU',
                                        'drvDDC2BI',
                                        'drvDEMOD',
                                        'drvDSCMB',
                                        'drvGPIO',
                                        'drvHWI2C',
                                        'drvIPAUTH',
                                        'drvIR',
                                        #'drvJPD',
                                        #'drvMMFI',
                                        #'drvMPIF',
                                        'drvMVOP',
                                        'drvPCMCIA',
                                        'drvPM',
                                        'drvPWM',
                                        'drvPWS',
                                        'drvRTC',
                                        'drvSAR',
                                        'drvSYS',
                                        'drvSERFLASH',
                                        #'drvTSP',
                                        'drvURDMA',
                                        #'drvUSB_HID_P1',
                                        #'drvUSB_HID_P2',
                                        #'drvUSB_HOST_P1',
                                        #'drvUSB_HOST_P2',
                                        'drvVBI',
                                        'drvVE',
                                        'drvVIF',
                                        'drvWDT',
                                        'drvMSPI',
                                        'EmAacLcEncLib_gcc4_7_2',
                                        'mrec',
                                        #'MMR',
                                        'venc',
                                        #'MMUtils',
                                        'drvMFE',
                                        'drvMFD',
                                        #'asound',
                                        'linux',
                                        'MsFS',
                                        'platform',
                                        'drvLDM'
                                        ])
            if not include_options.ice_tests and not include_options.challenger:
                utopia_dynamic_libs += ' -l'.join([
                                            ' -laudio.a2dp.default',
                                            'bluetooth.default',
                                            'bt-alsa-playback',
                                            'bt-common',
                                            'bt-mw',
                                            'btmw-test',
                                            'bt-vendor',
                                            'ipcrpc',
                                            'mtk_bt_ipcrpc_struct',
                                            'mtk_bt_service_client',
                                            'mtk_bt_service_server',
                                            ])
        else:
            utopia_dynamic_libs += ' -l'.join([
                                        '-lapiACE',
                                        'apiAUDIO',
                                        'apiCEC',
                                        'apiDLC',
                                        'apiDMX',
                                        'apiGFX',
                                        'apiGOP',
                                        'apiGPD',
                                        'apiJPEG',
                                        'apiMFE',
                                        'apiPNL',
                                        'apiSWI2C',
                                        'apiVDEC',
                                        #'apiVDEC_V1',
                                        'apiXC',
                                        'hdcp1x',
                                        'demoCA',
                                        'pvr_CA',
                                        'hdcp1xCA',
                                        'drvPQ',
                                        'drvAESDMA',
                                        #'drvAUDIO',
                                        'drvAUDSP',
                                        'drvAVD',
                                        'drvCPU',
                                        'drvDDC2BI',
                                        'drvDEMOD',
                                        'drvDSCMB',
                                        'drvGPIO',
                                        'drvHWI2C',
                                        'drvIPAUTH',
                                        'drvIR',
                                        #'drvJPD',
                                        #'drvMMFI',
                                        #'drvMPIF',
                                        'drvMVOP',
                                        'drvPCMCIA',
                                        'drvPM',
                                        'drvPWM',
                                        'drvPWS',
                                        'drvRTC',
                                        'drvSAR',
                                        'drvSYS',
                                        'drvSERFLASH',
                                        #'drvTSP',
                                        'drvURDMA',
                                        #'drvUSB_HID_P1',
                                        #'drvUSB_HID_P2',
                                        #'drvUSB_HOST_P1',
                                        #'drvUSB_HOST_P2',
                                        'drvVBI',
                                        'drvVE',
                                        'drvVIF',
                                        'drvWDT',
                                        'drvMSPI',
                                        'EmAacLcEncLib_gcc4_7_2',
                                        'mrec',
                                        #'MMR',
                                        'venc',
                                        #'MMUtils',
                                        'drvMFE',
                                        'drvMFD',
                                        #'asound',
                                        'linux',
                                        'MsFS',
                                        'audio.a2dp.default',
                                        'bluetooth.default',
                                        'bt-alsa-playback',
                                        'bt-common',
                                        'bt-mw',
                                        'btmw-test',
                                        'bt-vendor',
                                        'ipcrpc',
                                        'mtk_bt_ipcrpc_struct',
                                        'mtk_bt_service_client',
                                        'mtk_bt_service_server',
                                        'drvLDM'
                                        ])
    elif 'dynamic_lib' in conf_options and not include_options.connected:
        special_build(
            'release_lib_package',
            command = (

                            #"tar -xzf %s/mstar/mapi/hdcp2/arm-gnueabi/hardfloat/libhdcp_G36.tar.gz -C %s/mstar/mapi/hdcp2/" % (ice_source_directory, ice_source_directory),
                            #"cp -f %s/mstar/mapi/hdcp2/libhdcp/include/* %s/mstar/mapi/hdcp2/" % (ice_source_directory, ice_source_directory),
                            #"cp -f %s/mstar/mapi/hdcp2/libhdcp/lib/tee/* %s/mstar/bsp/%s/bin/" % (ice_source_directory, ice_source_directory, _mstar_bsp),
                            #"cp -f %s/mstar/mapi/hdcp2/libhdcp/lib/tee/* %s/environment/release/vendor/lib" % (ice_source_directory, project_build_dir),
                            "tar -xzf %s/mstar/ddi_smp/%s/libDDI_SMP-optee.tar.gz -C %s/mstar/ddi_smp/" % (ice_source_directory, _mstar_bsp, ice_source_directory),
                            "cp -f %s/mstar/ddi_smp/libDDI_SMP/lib/optee/* %s/vendor/lib" % (ice_source_directory, project_release_dir),
                            "tar -xzf %s/mstar/tee/MB181_OPTEE.tar.gz -C %s/mstar/tee/" % (ice_source_directory, ice_source_directory),
                            "cp -f %s/mstar/tee/bin/* %s/vendor/applications" % (ice_source_directory, project_release_dir),
                            "cp -rf %s/mstar/tee/config/* %s/vendor/config/" % (ice_source_directory, project_release_dir),
                            "cp -f %s/mstar/tee/image/* %s/environment/mscript/tee/" % (ice_source_directory, project_build_dir),
                            "cp -f %s/mstar/tee/lib/* %s/vendor/lib/" % (ice_source_directory, project_release_dir),
                            "mv -f %s/mstar/tee/optee_armtz %s/ROOTFS/lib/" % (ice_source_directory, project_release_dir),
                            "rm -rf %s/mstar/tee/bin" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/config" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/image" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/lib" % (ice_source_directory),
                            "rm -rf %s/mstar/tee/CusConfig" % (ice_source_directory),
                            'if [ %d -eq 1 ]; then $(call copyfile, %s/mstar/widi/*.config , %s/conf);fi' % (_include_widi_support,ice_source_directory,project_release_dir),
                            #'if [ %d -eq 1 ]; then $(call copyfile, %s/mstar/widi/sample_rtsp_sink , %s/conf);fi' % ( _include_widi_support,ice_source_directory,project_release_dir),
                            'if [ %d -eq 1 ]; then $(call copyfile, %s/mstar/widi/edid.edid , %s/conf);fi' % ( _include_widi_support,ice_source_directory,project_release_dir),
                            #'strings -a %s/mstar/bsp/%s/so_lib/libapiVDEC.so | grep "VESTEL" > %s/mstar/bsp/%s/so_lib/normal_vdec_version.txt' % (ice_source_directory, _mstar_bsp, ice_source_directory, _mstar_bsp),
                            #'if ! diff -q %s/mstar/tee/TEE_Release_MB130/secure_vdec_version.txt %s/mstar/bsp/%s/so_lib/normal_vdec_version.txt ; then exit 1; fi' % (ice_source_directory, ice_source_directory, _mstar_bsp),
                            "cd %s/mstar/customerinfo/;./genCusInfo %s;cd -"  % (ice_source_directory, _mstar_chip),
                            "cp -rf %s/mstar/customerinfo/libcusinfo.so %s/environment/release/vendor/lib"  % (ice_source_directory, project_build_dir),
                            "sh %s/mstar/bsp/%s/bin/shmver_test.sh %s/mstar/bsp/%s/bin %s/mstar/mmsdk/mmsdk/%s" % ( ice_source_directory, _mstar_bsp, ice_source_directory, _mstar_bsp, ice_source_directory, _mstar_bsp),
            ),
            required_by = cbuild_first,
        )
        pre_order += ' release_lib_package'
        utopia_ld_flags = ' -L' + os.path.join(project_build_dir, 'environment/release/vendor/lib')
        utopia_dynamic_libs += ' -l'.join([
                                    '-lapiACE',
                                    'apiAUDIO',
                                    'apiCEC',
                                    'apiDLC',
                                    'apiDMX',
                                    'apiGFX',
                                    'apiGOP',
                                    'apiGPD',
                                    'apiJPEG',
                                    'apiMFE',
                                    'apiPNL',
                                    'apiSWI2C',
                                    'apiVDEC',
                                    #'apiVDEC_V1',
                                    'apiXC',
                                    'hdcp1x',
                                    'demoCA',
                                    'pvr_CA',
                                    'hdcp1xCA',
                                    'drvPQ',
                                    'drvAESDMA',
                                    #'drvAUDIO',
                                    'drvAUDSP',
                                    'drvAVD',
                                    'drvCPU',
                                    'drvDDC2BI',
                                    'drvDEMOD',
                                    'drvDSCMB',
                                    'drvGPIO',
                                    'drvHWI2C',
                                    'drvIPAUTH',
                                    'drvIR',
                                    #'drvJPD',
                                    #'drvMMFI',
                                    #'drvMPIF',
                                    'drvMVOP',
                                    'drvPCMCIA',
                                    'drvPM',
                                    'drvPWM',
                                    'drvPWS',
                                    'drvRTC',
                                    'drvSAR',
                                    'drvSYS',
                                    'drvSERFLASH',
                                    #'drvTSP',
                                    'drvURDMA',
                                    #'drvUSB_HID_P1',
                                    #'drvUSB_HID_P2',
                                    #'drvUSB_HOST_P1',
                                    #'drvUSB_HOST_P2',
                                    'drvVBI',
                                    'drvVE',
                                    'drvVIF',
                                    'drvWDT',
                                    'drvMSPI',
                                    'EmAacLcEncLib_gcc4_7_2',
                                    'mrec',
                                    #'MMR',
                                    'venc',
                                    #'MMUtils',
                                    'drvMFE',
                                    'drvMFD',
                                    #'asound',
                                    'linux',
                                    'MsFS',
                                    'audio.a2dp.default',
                                    'bluetooth.default',
                                    'bt-alsa-playback',
                                    'bt-common',
                                    'bt-mw',
                                    'btmw-test',
                                    'bt-vendor',
                                    'ipcrpc',
                                    'mtk_bt_ipcrpc_struct',
                                    'mtk_bt_service_client',
                                    'mtk_bt_service_server',
                                    'drvLDM'
                                    ])
    else:
        utopia_static_libs += ' '.join([
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiACE.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiAUDIO.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiCEC.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiDLC.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiDMX.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiGFX.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiGOP.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiGPD.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiJPEG.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiMFE.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiPNL.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiSWI2C.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiVDEC.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libapiXC.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvAESDMA.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvAUDIO.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvAUDSP.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvAVD.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvCMDQ.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvCPU.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvDDC2BI.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvDEMOD.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvDIP.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvDSCMB.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvGPIO.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvHWI2C.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvIPAUTH.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvIR.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvMFE.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvMSPI.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvMVOP.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvPCMCIA.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvPM.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvPWM.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvPWS.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvRTC.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvSAR.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvSC.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvSERFLASH.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvTSP.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvURDMA.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvUSB_HID_P1.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvUSB_HID_P2.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvUSB_HID_P3.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvUSB_HOST_P1.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvUSB_HOST_P2.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvUSB_HOST_P3.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvVBI.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvVE.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvVIF.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvWDT.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libdrvMSPI.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/liblinux.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libMsFS.a',
                                    #ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libmrec_c.a',
                                    ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libmdlna.a',
                                    ])

    # There is not any corresponding .so file for mdlna and mrec_c
    # libmdlna.a and libmrec_c.a should be used for both dynamic and static builds.
    utopia_static_libs = ' '.join([
                                ice_source_directory + '/mstar/bsp/' + _mstar_bsp + '/lib/libmdlna.a',
    #                            ice_source_directory + '/mstar/mrec/lib/' + _mstar_bsp + '/libmrec_c.a',
    #                            ice_source_directory + '/mstar/mrec/lib/D8S/libmrec_c.a',
                                utopia_static_libs,
                             ])

    # There is not any corresponding .a file for vdplayer
    # libvdplayer.so should be used for both dynamic and static builds.
    utopia_dynamic_libs = ' '.join([
                                ' -lvdplayer',
                                ' -lusb-1.0',
                                widi_link_flag,
                                #'vendor/lib/libasound.so.2.0.0',
                                ice_source_directory + '/mstar/audio_alsa/lib/libasound.so.2.0.0',
                                utopia_dynamic_libs,
                             ])

    ###############################################################################

    ###############################################################################

    strip = TOOLCHAIN_PATH + '/arm-linux-gnueabi-strip'

    ###############################################################################

    _link_requires = 'vendor'

    _link_requires += ' copy_application_scripts'

#    if include_options.bluetooth:
#        _link_requires += ' copy_bluetooth_server'

    _link_requires += ' copy_default_ui'

    if include_options.dlna:
        _link_requires += ' copy_dmr_files'
        if not include_options.nflc30:
            _link_requires += ' copy_dms_files'

    if include_options.dial:
        _link_requires += ' copy_dial_files'

    #_link_requires += ' create_debug_scripts'

    ############# Vewd Browser build section #####################################
    if include_options.vewd_browser:
        use_component('vewd_common')
        note('Vewd source package is %s' % (vewd_sdk_package_file))
        use_interface(os.path.join(vewd_sdk_package_location, vewd_sdk_package_file_name))
        use_external_component(vewd_font_location)

        jsplugins = [
            'vewd_common/jsplugins/bt_voice', 
            'vewd_common/jsplugins/drm_data_loader', 
            'vewd_common/jsplugins/macid',
            'vewd_common/jsplugins/profile_status',
            'vewd_common/jsplugins/system_info_object',
            'vewd_common/jsplugins/tivo_os',
            'vewd_integration/jspp_plugins/ApplicationController',
            'vewd_integration/jspp_plugins/AVSClient',
            'vewd_integration/jspp_plugins/MacId',
            'vewd_integration/jspp_plugins/DigitalIBHelper'
        ]

        if include_options.vewd_sdk423:
            use_component('vewd_common/ipc')

            if not vewd_user_build:
                use_component('3rd_party_wrapper/rpclib')
                use_component('nebula_src/adaptation_layer/ipc')

                if include_options.opapp:
                    use_component('nebula_src/opapp/ipc')

                # Build mumin only files for a shared library.
                use_component('vewd_integration/ipc')
                use_component('vewd_integration/drm_backends/ciplus')

                for plugin in jsplugins:
                    use_component(plugin)

                # Build 'shared' files for a shared library.
                use_component('vewd_integration/ipc/shared/frost_src', 
                        PLATFORM_FROST_SRC_DIR = 'ice_product_mstar_g3x')
                use_component('vewd_integration/ipc/shared/http_client_src')
                use_component('vewd_integration/ipc/shared/language_src')
                use_component('vewd_integration/ipc/shared/nebula_src')
                use_component('vewd_integration/ipc/shared/utilities_public_src')
                use_component('vewd_integration/ipc/shared/utilities_private_src')
                use_component('vewd_integration/ipc/shared/utilities_debug_src')
                use_component('vewd_integration/ipc/shared/vewd_integration',
                       VEWD_SDK_DIST = vewd_sdk_dist_dir)

        use_component(
            vewd_module,
            VEWD_SDK_DIST = vewd_sdk_dist_dir,
            ENABLE_DIRECTFB_BACKEND = None,
            ENABLE_OPENGLES_BACKEND = True,
            ENABLE_BROADCAST_RESOURCE_MANAGEMENT = False,
            USE_MEDIA_PLAYER_WINDOW = include_options.vewd_sdk423
        )

    vewd_libs = ''
    vewd_ld_flags = ''

    _digital_ib_path = 'digital_ib_orion'
    if _include_html_applications:
        use_external_component(os.path.join('html_applications', _digital_ib_path))

    if _include_html_app_sdk_tests:
        use_external_component('html_app_sdk_tests')
    
    if _include_lighttpd_web_server:
        use_external_component(os.path.join('html_applications', 'tivo'))
        use_external_component(os.path.join('html_applications', 'html_ui'))
        if vewd_user_build or mstar_user_build:
            use_interface('3rd_party/lighttpd/lighttpd_tivo.conf')
            use_interface('3rd_party/lighttpd/lighttpd_tivo_prestaging.conf')
        use_external_component(os.path.join('html_applications', 'hoteltv'))
        use_external_component(os.path.join('html_applications', 'rfhoteltv'))
        if vewd_user_build or mstar_user_build:
            use_interface('3rd_party/lighttpd/lighttpd_hoteltv.conf')


    if include_options.vewd_browser:
        vewd_libs += ' -lnss3 -lssl3 -luva -lnssutil3 -lsmime3 -lplc4 -lnspr4 -lplds4'
        if include_options.vewd_sdk423:
            vewd_libs += ' -lomi -lmpc_api -lffmpeg -luva2_hld -lmxml -lcap'
        else:
            vewd_libs += ' -lomi -lmpc_api -lnetview -lffmpeg -luva2_hld -luvatesting -lmxml -lcap'

        vewd_ld_flags += ' -L' + os.path.join(project_root, vewd_sdk_dist_dir, 'lib')
        vewd_ld_flags += ' -L' + os.path.join(project_release_dir, 'vendor/lib')
        vewd_ld_flags += ' -L' + os.path.join(project_release_dir, 'ROOTFS/usr/lib')

    if include_options.use_gles or include_options.gles_ui:
        libDFB_ld_flag += ' -lmali'
 
    jwt_libs = ''
    jansson_libs = ''

    if _include_extended_logging:
        jwt_libs = "%s/libjwt.a" % (os.path.join(pubsub_tmp_directory, "3rd_party/lib"))
        jansson_libs = "%s/libjansson.a" % (os.path.join(pubsub_tmp_directory, "3rd_party/lib"))

    resource_libs = ""
    if not include_options.dynamic_ui and is_component_used("app_orion"):
        resource_libs = '$(derived_dir)/app_orion/libDemoResourceTable_font.a'
        resource_libs += ' $(derived_dir)/app_orion/libDemoResourceTable_image.a'

    ###############################################################################
    if include_options.connected:
        mmsdk_subtitle = '-lttx_hf -ldlmalloc'
        if include_options.ice_tests or include_options.challenger:
            mmsdk = '-lmmsdk -lmmsdk_porting -lmmp -lmmp_asf -lMMUtils -lapiDMS -lteec -lOMX.MStar -lstagefright_soft_mstaacdec -lstagefrighthw -lvsyncbridge -lstagefrighthw_dip -lcapturemode -lmmutils_log -lDDI_SMP'
        else:
            mmsdk = '-lmmsdk -lmmsdk_porting -lmmp -lmmp_asf -lMMUtils -lapiDMS -liniparser -lteec -lOMX.MStar -lstagefright_soft_mstaacdec -lstagefrighthw -lvsyncbridge -lstagefrighthw_dip -lcapturemode -lmmutils_log -lDDI_SMP'
    else:
        mmsdk = '-lmmsdk -lmmsdk_porting'
        mmsdk_subtitle = ' '


    def verify_shared_library(build_name, plugin=None, required_by=None):
        ''' Generate a special build to verify that the mumin backend or plugin has no 
            unexpected symbols.

            If the required_by parameter is not provided then the verify_target string 
            returned from this function must be added to the 'requires' parameter of some 
            other special_build otherwise the Makefile recipe generated by the special_build 
            here will not be executed.

        Parameters:
            build_name  The build alias of the special_build that generated the library to be verified.
            plugin      The path/filename of the library to be verified if it is a plugin.
            required_by Build alias of another build which causes this to be executed.
        '''

        mumin_backend_dir = os.path.join('$(derived_dir)', 'uva_backend')
        verify_script = os.path.join(project_directory, 'scripts', 'vewd_missing_browser_symbols.py')

        cmd = [verify_script, '-v', vewd_sdk_dist_dir, 
                              '-l', mumin_backend_dir, 
                              '-n', os.path.join(toolchain_dir,'arm-linux-gnueabi/bin/nm'),
                              '-p', 'mb181',
                              '-t', toolchain_dir]
        if plugin:
            cmd += ['-u', plugin]

        verify_target = 'verify_' + build_name

        special_build(
            verify_target,
            command = (' '.join(cmd),),
            requires = build_name,
            required_by = required_by,
            phony = True
        )

        return verify_target


    def create_mumin_backend():
        """Create the mumin uva backend dynamic library.

        Create the libuvabackend-mumin.so by linking all of the required static libraries. Copy
        it to browser_dir.

        Returns:
            A string containing (makefile variables that reference) all static libraries that
            make up the generated dynamic library.

        """

        if vewd_user_build or mstar_user_build or amazon_user_build:
            libs_mumin = ' '.join([
                'libs_mumin/$(call libname,ipc)',
                'libs_mumin/$(call libname,frost_src)',
                'libs_mumin/$(call libname,http_client_src)',
                'libs_mumin/$(call libname,language_src)',
                'libs_mumin/$(call libname,nebula_src)',
                'libs_mumin/$(call libname,utilities_public_src)',
                'libs_mumin/$(call libname,utilities_private_src)',
                'libs_mumin/$(call libname,utilities_debug_src)',
                'libs_mumin/$(call libname,vewd_integration)',
            ])
        else:
            libs_mumin = ' '.join([
                '$(derived_dir)/vewd_integration/ipc/$(call libname,ipc)',
                '$(derived_dir)/vewd_integration/ipc/shared/frost_src/$(call libname,frost_src)',
                '$(derived_dir)/vewd_integration/ipc/shared/http_client_src/$(call libname,http_client_src)',
                '$(derived_dir)/vewd_integration/ipc/shared/language_src/$(call libname,language_src)',
                '$(derived_dir)/vewd_integration/ipc/shared/nebula_src/$(call libname,nebula_src)',
                '$(derived_dir)/vewd_integration/ipc/shared/utilities_public_src/$(call libname,utilities_public_src)',
                '$(derived_dir)/vewd_integration/ipc/shared/utilities_private_src/$(call libname,utilities_private_src)',
                '$(derived_dir)/vewd_integration/ipc/shared/utilities_debug_src/$(call libname,utilities_debug_src)',
                '$(derived_dir)/vewd_integration/ipc/shared/vewd_integration/$(call libname,vewd_integration)',
            ])

        mumin_backend = os.path.join('$(derived_dir)', 'uva_backend', 'libuvabackend-mumin.so')

        mumin_third_party_libs = (rpclib_lib,)
        special_build(
            mumin_backend,
            command = ( 'mkdir -p {}'.format(os.path.dirname(mumin_backend)), 
                        '$(call link_shared,$@,{}, {} {})'.format(
                        '', #objs.
                        libs_mumin,
                        ' '.join(mumin_third_party_libs)),
            ),
            requires = '{} {}'.format(libs_mumin, ' '.join(mumin_third_party_libs)),
            aliases = 'mumin_backend',
            required_by = 'elf'
        )

        verify_shared_library('mumin_backend', required_by='elf')

        release_browser_dir = os.path.join(project_release_dir, 
                                            'vendor', 'browser_dir', 'uva_ooif_backends')
        special_build(
            'copy_mumin_backend',
            command = ('cp -f {} {}'.format(mumin_backend, release_browser_dir)),
            # Logically copy_vewd_dirs is also a requirement, but adding this causes extract_vewd
            # and other phony targets to be re-executed, which can cause build issues.
            requires = 'mumin_backend',
            required_by = 'elf',
            phony = 1
        )

        return libs_mumin


    def create_ciplus_drm_backend():
        """Create the CI+ DRM backend library.

        Returns:
            A string containing (makefile variables that reference) all object files that
            make up the generated library.
        """

        build_name = 'ciplus_drm_backend'
        if vewd_user_build or mstar_user_build:
            libs_drm_backend = ' '.join([
                'libs_drm_backends/$(call libname,ciplus)',])
        else:
            libs_drm_backend = ' '.join([
                '$(derived_dir)/vewd_integration/drm_backends/ciplus/$(call libname,ciplus)',])

        drm_backend = os.path.join(
                            '$(derived_dir)', 'vewd_integration', 'drm_backends', 'ciplus',
                            'libuvadrmbackend-ciplus.so')
    
        third_party_libs = (rpclib_lib,)
        uva_backend_lib = ('-L {lp} -Xlinker -rpath-link={lp} -luvabackend-mumin'.format(
                                lp=os.path.join(project_root, '$(derived_dir)', 'uva_backend')),)
        special_build(
            drm_backend,
            command = ('mkdir -p {}'.format(os.path.dirname(drm_backend)), 
                        '$(call link_shared,$@,{}, {} {})'.format(
                            '', #objs.
                            libs_drm_backend,
                            ' '.join(third_party_libs + uva_backend_lib)),
            ),
            requires = '{} {}  mumin_backend'.format(libs_drm_backend, ' '.join(third_party_libs)),
            aliases = build_name,
        )
        verify_shared_library(build_name, plugin=drm_backend)

        release_browser_dir = os.path.join(project_release_dir, 
                                            'vendor', 'browser_dir', 'uva_drm_backends')
        special_build(
            'copy_ciplus_drm_backend',
            command = ('cp -f {} {}'.format(drm_backend, release_browser_dir)),
            requires = build_name,
            required_by = 'elf',
            phony = 1
        )
        return libs_drm_backend


    def create_all_jsplugins(jsplugins, static_library_path):
        """Create all jsplugin libraries.

        Create all jsplugin libraries based on the supplied jsplugins list. Update the
        jsplugins.json file to reference all plugins.

        Parameters:
            jsplugins - A list of plugin paths.
                (e.g. 'vewd_common/jsplugins/profile_status')
            static_library_path - A path to override where the static libraries can
            be found.

        Returns:
            A string containing (makefile variables that reference) all static libraries that
            make up the generated dynamic libraries.

        """

        plugin_dir = os.path.join(project_release_dir, 'vendor', 'browser_dir', 'jsplugins')

        all_jsplugin_builds = ''
        all_jsplugin_libs = ''
            
        for plugin in jsplugins:
            plugin_build_name, libs_plugin = (
                create_jsplugin(plugin_dir, plugin, static_library_path)
            )
            all_jsplugin_builds += ' ' + plugin_build_name
            all_jsplugin_libs += ' ' + libs_plugin

        create_jsplugin_json(plugin_dir, all_jsplugin_builds)

        return all_jsplugin_libs


    def create_jsplugin(plugin_dir, plugin, static_library_path):
        """Create a jsplugin library.

        Create a jsplugin library (.so) based on the supplied plugin.

        Parameters:
            plugin_dir - The path to output the plugin library to.
            plugin - The main path of the plugin to be built.
            static_library_path - A path to override where the the static libraries can
            be found, otherwise they are assumed to be in $(derived_dir).

        Returns: 
            The build name of the library generation.
            A string containing (makefile variables that reference) all static libraries that
            make up the generated dynamic library.

        """

        # Use the plugin directory as the plugin name.
        plugin_name = subfolder(plugin)
        build_name = '{}_plugin'.format(plugin_name)

        libs_list = []
        if static_library_path:
            libs_list.append(os.path.join(static_library_path, plugin_name, 
                                            libname(plugin_name)))
        else:
            libs_list.append(os.path.join('$(derived_dir)', plugin, libname(plugin_name)))

        libs = ' '.join(libs_list)

        plugin_library = os.path.join(plugin_dir, 'lib{}-plugin.so'.format(plugin_name))
        plugin_third_party_libs = (rpclib_lib,)
        uva_backend_lib = ('-L {lp} -Xlinker -rpath-link={lp} -luvabackend-mumin'.format(
                                lp=os.path.join(project_root, '$(derived_dir)', 'uva_backend')),)

        special_build(
            plugin_library,
            command = ('$(call link_shared,$@,{}, {} {})'.format(
                            '', #objs.
                            libs,
                            ' '.join(plugin_third_party_libs + uva_backend_lib))),
            requires = '{} {} mumin_backend'.format(libs, ' '.join(plugin_third_party_libs)),
            aliases = build_name
        )

        verify_shared_library(build_name, plugin=plugin_library, required_by='jsplugin_json')

        return build_name, libs


    def create_jsplugin_json(plugin_dir, jsplugin_builds):
        """Update the jsplugins.json.
        
        Update the jsplugins.json to reference all plugins in the plugin_dir.

        Parameters:
            plugin_dir - The path to where the plugin libraries are stored. 
                         The path to output the jsplugin.json file.
            jsplugin_builds - A string containing all the build aliases of plugin library
                              generation.

        """

        build_name = 'jsplugin_json'
        json_plugin_script = os.path.join(project_build_dir, 'scripts',
                                        'generate_jsplugin_json.py')

        _generate_jsplugin_json = 1
        if mstar_user_build:
            _generate_jsplugin_json = 0

        special_build(
            build_name,
            command = ('if [ {} -eq 1 ]; then python {} {}; fi'.format(_generate_jsplugin_json, json_plugin_script, plugin_dir)),
            requires = jsplugin_builds,
            required_by = 'elf',
            aliases = 'create_{}'.format(build_name)
        )

    if include_options.vewd_sdk423:
        libs_mumin = create_mumin_backend()
        libs_drm_backend = create_ciplus_drm_backend()

        static_library_path = None
        if vewd_user_build or mstar_user_build or amazon_user_build:
            static_library_path = EXTERNAL_DELIVERY_PLUGINS_DIR

        libs_plugins = create_all_jsplugins(jsplugins, static_library_path)

        # Do not include the static libraries in the elf, that are built exclusively for
        # the mumin or drm backend or plugin shared libraries.
        all_static_libs = ' '.join(['$(filter-out {} {} {}, $(LIBS))'.format(libs_mumin,
                                                                          libs_drm_backend,
                                                                          libs_plugins)])

    all_ld_flags = ' '.join(
        [
            openssl_ld_flags,
            png_ld_flags,
            freetype_ld_flags,
            xpca_ld_flags,
            utopia_ld_flags,
            vewd_ld_flags,
            libDFB_ld_flag,
            apm_ld_flags,
            discretix_ld_flag,
            vmx_web_ld_flag,
            avs_ld_flag,
            aws_iot_ld_flag,
            libwebsockets_ld_flags,
            all_ld_flags,
        ])

    all_dynamic_libs = ' '.join(
        [
            openssl_libs,
            png_libs,
            freetype_libs,
            zlib_libs,
            xpca_libs,
            avs_dynamic_lib,
            aws_iot_dynamic_lib,
            utopia_dynamic_libs,
            vewd_libs,
            libDFB,
            libbsa,
            liblive555,
            quickshare,
            libAPM,
            libwebsockets_libs,
            discretix_libs,
            vmx_web_libs,
            mmsdk,
            mmsdk_subtitle,
            nghttp2_dynamic_libs,
            all_dynamic_libs,
            optee_libs,
            playfi_libs,
         ])

    all_static_libs = ' '.join(
        [
            dlna_libs,
            utopia_static_libs,
            curl_platform_libs,
            libwifi,
            libxml2_lib,
            jwt_libs,
            jansson_libs,
            avs_static_libs,
            resource_libs,
            all_static_libs,
        ])

    if include_options.vewd_sdk423:
        all_static_libs += ' ' + rpclib_nopic_lib

    libs_txt = 'external_user_libs.txt'
    aurora_lib = 'aurora.a'
    no_access_txt = 'no_access_list.txt'

    external_user_icetest_build = False
    if(vewd_user_build or mstar_user_build or amazon_user_build):
        if include_options.ice_tests or include_options.challenger:
            special_build(
                'extract_aurora',
                command = (
                    "rm -rf %s/external_delivery/aurora; mkdir %s/external_delivery/aurora" % (project_build_dir, project_build_dir),
                    "ar -x %s/external_delivery/%s --output %s/external_delivery/aurora" % (project_build_dir, aurora_lib, project_build_dir),
                ),
                phony = True,
            )
            all_static_libs = ' '.join(['$(LIBS)',
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/ProfileHelper.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/ProfileManager.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/trace.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/StringUtility.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/UString.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/FrostMutex.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/ExplodingBoolean.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/utf8.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'aurora/Ucs2CharacterMap.o'),
                            '%s/external_delivery/%s' % (project_build_dir, 'project/libcurl/output/mb181/openssl/lib/libcurl.a'),
                            all_static_libs
                            ])
            external_user_icetest_build = True
        else:
            all_static_libs = ' '.join(['$(LIBS)',
                            '%s/external_delivery/%s' % (project_build_dir,aurora_lib),
                            all_static_libs
                            ])
    if not external_user_icetest_build:
        special_build(
            'extract_aurora',
            phony = True,
        )

    if mstar_user_build:
        all_static_libs = ' '.join(
            [
                ice_source_directory + '/cabot_libs/siI9287b.a',
                all_static_libs,
            ])

    special_build(
        'buildall',
        phony = 1,
    )

    all_libs = ' '.join(
        [
            all_dynamic_libs,
            all_static_libs,
        ])

    link_opts = '%s %s' % (all_ld_flags, all_libs)

    ###############################################################################
    elf_file_tocopy = '$(derived_dir)/aurora.elf'
    if include_options.netfront41:
        access_browser_path = 'AccessNFB_4.1'
    elif include_options.netfront42:
        access_browser_path = 'AccessNFB_4.2'
    else:
        access_browser_path = 'NOTVALID'

    _bin_file_name_option=''
    if include_options.ice_tests:
        _bin_file_name_option='_ice_tests'
    elif include_options.challenger:
        _bin_file_name_option='_challenger'
    elif not include_options.hbbtv and not include_options.portal and not include_options.iplayer:
        _bin_file_name_option='_nc'
    elif not include_options.gles_ui:
        _bin_file_name_option="_hal"



    make_link_func = 'link' if include_options.vewd_browser else 'link_static'

    # The aurora.elf in the derived directory is no longer stripped; the version
    # that's flashed or in nfsroot is.

    special_build(
        '$(derived_dir)/aurora.elf',
        component = '$(conf_dir)',
        command = (
            '$(call {0},$@, {1})'.format(make_link_func, link_opts),
            # create aurora_not_strip file as a link to avoid copying hundreds of
            # MBs.  (The elf file in the derived directory is no longer stripped.)
            'ln -f $(derived_dir)/aurora.elf {0}/aurora_not_strip{1}.elf'.format(
                                                            project_build_dir,
                                                            _bin_file_name_option),
        ),
        requires = ' '.join(['extract_aurora', all_static_libs, 'copy_dynamic_lib']),
        aliases = 'elf',
    )
    if _include_hotel_tv:
        special_build(
            'hoteltv_tools',
            command = ("mkdir -p %s/conf/hoteltv" % (project_release_dir) ,
                       "mkdir -p %s/conf/hoteltv/ip_tuner" % (project_release_dir) ,
                       "mkdir -p %s/conf/hoteltv/bootcaster" % (project_release_dir) ,
                       "mkdir -p %s/vendor/hoteltv/bootcaster" % (project_release_dir) ,
                       "cp -f hoteltv_tools/bootcaster/built/ReleaseBootCaster/start_dhcpcd.sh %s/vendor/hoteltv/bootcaster" % (project_release_dir),
                       "cp -f hoteltv_tools/bootcaster/built/ReleaseBootCaster/start_bootcaster.sh %s/vendor/hoteltv/bootcaster" % (project_release_dir),
                       "cp -f hoteltv_tools/bootcaster/built/ReleaseBootCaster/parse_bootcaster %s/vendor/hoteltv/bootcaster" % (project_release_dir),
                       "cp -f hoteltv_tools/bootcaster/built/ReleaseBootCaster/bootcaster %s/vendor/hoteltv/bootcaster" % (project_release_dir),
                       "chmod +x %s/vendor/hoteltv/bootcaster/start_dhcpcd.sh" % (project_release_dir),
                       "chmod +x %s/vendor/hoteltv/bootcaster/start_bootcaster.sh" % (project_release_dir),
                       "chmod +x %s/vendor/hoteltv/bootcaster/parse_bootcaster" % (project_release_dir),
                       "chmod +x %s/vendor/hoteltv/bootcaster/bootcaster" % (project_release_dir),
                       "mkdir -p %s/vendor/html_applications/rfhoteltv" % (project_release_dir),
                       "mkdir -p %s/vendor/html_applications/rfhoteltv/buffer/" % (project_release_dir),
                       "cp -rf html_applications/rfhoteltv %s/vendor/html_applications" % (project_release_dir),
                       "ln -sf /tmp/rfhoteltv %s/vendor/html_applications/rfhoteltv/tmp" % (project_release_dir),
                       "ln -sf /buffer/public/rfhoteltv %s/vendor/html_applications/rfhoteltv/buffer/public" % (project_release_dir),
                       "mkdir -p %s/vendor/html_applications/html_ui" % (project_release_dir),
                       "cp -rf html_applications/html_ui %s/vendor/html_applications" % (project_release_dir),
                       "if [ %d -eq 1 ]; then cp -rf  %s/3rd_party/lighttpd/lighttpd_hoteltv.conf %s/vendor;fi" % (_include_lighttpd_web_server, project_root, project_release_dir),
                       "cp %s/3rd_party/lighttpd/start_webserver.sh %s/vendor/" % (project_root, project_release_dir),
                       "ln -sf /buffer/public/custom_html_ui/ %s/vendor/html_applications/iphoteltv" % (project_release_dir),
                       "pushd .; cd %s; zip -r -q html_app_sdk_tests.zip html_app_sdk_tests; popd;" % (project_root),
                      ),
            required_by = cbuild_first,
            requires = pre_order,
            phony = True,
            )

        pre_order += ' hoteltv_tools'

    if include_options.dynamic_lib:
        special_build(
            'copy_dynamic_lib',
            # The symbolic link targets must be relative so that they can also be
            # followed on desktop linux (when examining a core file).
            command = (
                        'if [ ! -d %s/vendor/lib ];then mkdir %s/vendor/lib;fi' % (project_release_dir,project_release_dir),
                        '$(call copyfile, %s/mstar/bsp/%s/so_lib/* , %s/vendor/lib)' % (ice_source_directory, _mstar_bsp, project_release_dir),
                        "cp -Rf %s/mstar/audio_alsa/lib/libasound.so.2.0.0 %s/vendor/lib" % (ice_source_directory, project_release_dir),
                        "ln -sf libasound.so.2.0.0 %s/vendor/lib/libasound.so.2" % (project_release_dir),
                        "ln -sf libasound.so.2.0.0 %s/vendor/lib/libasound.so" % (project_release_dir),
                        #----------- Wifi -------------------------
                        "if [ %d -eq 1 ]; then cp -Rf %s/wifi/libusb-0.1.so.4.4.4 %s/vendor/lib;fi" % (_include_wifi, ice_source_directory, project_release_dir),
                        "if [ %d -eq 1 ]; then ln -f -s ./libusb-0.1.so.4.4.4 %s/vendor/lib/libusb.so;fi" % (_include_wifi, project_release_dir),
                        "if [ %d -eq 1 ]; then ln -f -s ./libusb-0.1.so.4.4.4 %s/vendor/lib/libusb-0.1.so.4;fi" % (_include_wifi, project_release_dir),
                        "if [ %d -eq 1 ]; then cp -Rf %s/wifi/libusb-1.0.so.0.1.0 %s/vendor/lib;fi" % (_include_wifi, ice_source_directory, project_release_dir),
                        "if [ %d -eq 1 ]; then ln -f -s ./libusb-1.0.so.0.1.0 %s/vendor/lib/libusb-1.0.so;fi" % (_include_wifi, project_release_dir),
                        "if [ %d -eq 1 ]; then ln -f -s ./libusb-1.0.so.0.1.0 %s/vendor/lib/libusb-1.0.so.0;fi" % (_include_wifi, project_release_dir),
                        'if [ %d -eq 1 ]; then $(call copyfile, -f %s/wifi/mt7668/arm-linux-gnueabi_5.5.0/lib/libnl-3.so.200.19.0 %s/vendor/lib/);fi' % (_include_wifi, ice_source_directory, project_release_dir),
                        'if [ %d -eq 1 ]; then $(call copyfile, -f %s/wifi/mt7668/arm-linux-gnueabi_5.5.0/lib/libnl-cli-3.so.200.19.0 %s/vendor/lib/);fi' % (_include_wifi, ice_source_directory, project_release_dir),
                        'if [ %d -eq 1 ]; then $(call copyfile, -f %s/wifi/mt7668/arm-linux-gnueabi_5.5.0/lib/libnl-genl-3.so.200.19.0 %s/vendor/lib/);fi' % (_include_wifi, ice_source_directory, project_release_dir),
                        'if [ %d -eq 1 ]; then $(call copyfile, -f %s/wifi/mt7668/arm-linux-gnueabi_5.5.0/lib/libnl-idiag-3.so.200.19.0 %s/vendor/lib/);fi' % (_include_wifi, ice_source_directory, project_release_dir),
                        'if [ %d -eq 1 ]; then $(call copyfile, -f %s/wifi/mt7668/arm-linux-gnueabi_5.5.0/lib/libnl-nf-3.so.200.19.0 %s/vendor/lib/);fi' % (_include_wifi, ice_source_directory, project_release_dir),
                        'if [ %d -eq 1 ]; then $(call copyfile, -f %s/wifi/mt7668/arm-linux-gnueabi_5.5.0/lib/libnl-route-3.so.200.19.0 %s/vendor/lib/);fi' % (_include_wifi, ice_source_directory, project_release_dir),
                        'if [ %d -eq 1 ]; then $(call copyfile, -f %s/wifi/mt7668/arm-linux-gnueabi_5.5.0/lib/libssl.so.1.0.0 %s/vendor/lib/);fi' % (_include_wifi, ice_source_directory, project_release_dir),
                        'if [ %d -eq 1 ]; then $(call copyfile, -f %s/wifi/mt7668/arm-linux-gnueabi_5.5.0/lib/libcrypto.so.1.0.0 %s/vendor/lib/);fi' % (_include_wifi, ice_source_directory, project_release_dir),
                        #----------- Bluetooth -------------------------
                        'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/lib/* , %s/environment/release/vendor/lib/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
                        #----------- Bluetooth end -------------------------
                        "cp -Rf %s/mstar/mrec/lib/libEmAacLcEncLib_gcc4_7_2.so %s/vendor/lib" % (ice_source_directory, project_release_dir),
                        "ln -sf libEmAacLcEncLib_gcc4_7_2.so %s/vendor/lib/libEmAacLcEncLib_gcc4_7_2.so.1" % (project_release_dir),
                        "cp -Rf %s/mstar/mrec/lib/%s/libmrec.so %s/vendor/lib" % (ice_source_directory, _mstar_bsp, project_release_dir),
                        #"cp -Rf %s/mstar/mrec/lib/%s/libMMR.so %s/vendor/lib" % (ice_source_directory, _mstar_bsp, project_release_dir),
                        "cp -Rf %s/mstar/mrec/lib/%s/libvenc.so %s/vendor/lib" % (ice_source_directory, _mstar_bsp, project_release_dir),
                       # "cp -Rf %s/mstar/mrec/lib/%s/libMMUtils.so %s/vendor/lib" % (ice_source_directory, _mstar_bsp, project_release_dir),
                      #  "cp -Rf %s/mstar/bsp/%s/bin/libcrypto.so.0.9.8 %s/vendor/lib" % (ice_source_directory, _mstar_bsp, project_release_dir),
                        "mkdir -p %s/vendor/etc" % (project_release_dir),
                        "cp -f %s/mstar/bsp/%s/utopia.conf %s/vendor/etc" % (ice_source_directory, _mstar_bsp, project_release_dir),
                        "cp -Rf %s/mstar/bsp/%s/bin/*.so %s/vendor/lib" % (ice_source_directory, _mstar_bsp, project_release_dir),
                        "cp -Rf %s/mstar/bsp/%s/bin/cpu_audio %s/vendor/lib" % (ice_source_directory, _mstar_bsp, project_release_dir),
                        'if [ ! -d %s/environment/release/vendor/lib ];then mkdir -p %s/environment/release/vendor/lib;fi' % (project_build_dir,project_build_dir),
                        '$(call copyfile, %s/mstar/mmsdk/mmsdk/%s/*.so , %s/vendor/lib/)' % (ice_source_directory,_mstar_bsp,project_release_dir),
                        '$(call copyfile, %s/mstar/mmsdk/portinglayer/%s/*.so , %s/vendor/lib/)' % (ice_source_directory ,_mstar_bsp, project_release_dir),
                        '$(call copyfile, %s/mstar/dlmalloc/*.so , %s/vendor/lib/)' % (ice_source_directory,project_release_dir),
                        '$(call copyfile, %s/environment/distribution/vendor/config/*.ini , %s/vendor/config/)' % (project_build_dir,project_release_dir),
                        "if [ %d -eq 1 ]; then cp -Rf %s/mstar/widi/mb181/libmiracastrx.so %s/vendor/lib;fi" % (_include_widi_support, ice_source_directory, project_release_dir),
                        'if [ %d -eq 1 ]; then mkdir -p %s/vendor/config/wifi_p2p;fi' % (_include_widi_support, project_release_dir),
                        "if [ %d -eq 1 ]; then cp -Rf %s/mstar/widi/mb181/wifi_p2p.ini %s/vendor/config/wifi_p2p;fi" % (_include_widi_support, ice_source_directory, project_release_dir),
                        "if [ %d -eq 1 ]; then cp -Rf %s/mstar/widi/mb181/lib/* %s/vendor/lib;fi" % (_include_widi_support, ice_source_directory, project_release_dir),
                        #"if [ %d -eq 1 ]; then cp -Rf %s/mstar/widi/mb181/lib_wifip2p/non-root/* %s/vendor/lib;fi" % (_include_AMZ_support, ice_source_directory, project_release_dir),
                        "if [ %d -eq 0 ]; then cp -Rf %s/mstar/widi/mb181/lib_wifip2p/root/* %s/vendor/lib;fi" % (0, ice_source_directory, project_release_dir),
                      ),
            phony = True,
        )
    else:
        special_build(
            'copy_dynamic_lib',
            phony = True,
                )

    vendor_commands = [
            '%s/scripts/make_vendor_dirs.sh %s %s' % (project_build_dir, project_directory, mb_name),
            '$(call copyfile, %s, %s/vendor)' % (elf_file_tocopy, project_release_dir),
            '$(call copyfile, $(derived_dir)/ca-bundle.crt, %s/vendor)' % (project_release_dir),
            #'$(call copyfile, %s/Main_Text.bin , %s/conf)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/%s/Main.bin , %s/conf)' % (config_directory, _mstar_chip, project_release_dir),
            '$(call copyfile, %s/%s/Main_*.bin , %s/conf)' % (config_directory, _mstar_chip, project_release_dir),
            #'rm -rf %s/conf/Main_Color*' % (project_release_dir),
            '$(call copyfile, %s/%s/Sub.bin , %s/conf)' % (config_directory, _mstar_chip, project_release_dir),
            '$(call copyfile, %s/%s/Sub_*.bin , %s/conf)' % (config_directory, _mstar_chip, project_release_dir),
            '$(call copyfile, %s/%s/HSY.bin , %s/conf)' % (config_directory, _mstar_chip, project_release_dir),
            #'$(call copyfile, %s/%s_*.bin , %s/conf)' % (config_directory, _mstar_chip, project_release_dir ),
            #'$(call copyfile, %s/dolby.bin , %s/conf)' % (config_directory, project_release_dir),
            'if [ %s = "G32" ]; then $(call copyfile, %s/dolby.bin , %s/conf);fi' % (_mstar_chip, config_directory, project_release_dir),
            'if [ %s = "G31" ]; then $(call copyfile, %s/dolby.bin , %s/conf);fi' % (_mstar_chip, config_directory, project_release_dir),
            'if [ ! -d %s/ROOTFS/data/ ];then mkdir -p %s/ROOTFS/data/;fi' % ( project_release_dir, project_release_dir),
            'if [ ! -d %s%s ];then mkdir -p %s%s;fi' % (project_release_dir, _playready30_cer_path, project_release_dir, _playready30_cer_path),
            'if [ ! -d %s%s ];then mkdir -p %s%s;fi' % (project_release_dir, _youtube_cer_path, project_release_dir, _youtube_cer_path),
            'if [ ! -d %s%s ];then mkdir -p %s%s;fi' % (project_release_dir, _widevine_cer_path, project_release_dir, _widevine_cer_path),
            'if [ ! -d %s%s ];then mkdir -p %s%s;fi' % (project_release_dir, _ciecp_cer_path, project_release_dir, _ciecp_cer_path),
            "ln -sT /conf/tvcertificate %s/vendor/tvcertificate" % (project_release_dir),
            "cp -f %s/drm_data/credentials/vestel_pr40_model_credentials/bgroupcert.dat %s%s/bgroupcert.dat" % (config_directory, project_release_dir, _playready30_cer_path),
            "cp -f %s/drm_data/credentials/vestel_pr40_model_credentials/enc_zgpriv.dat %s%s/zgpriv_protected.dat" % (config_directory, project_release_dir, _playready30_cer_path),
            'if [ %s = "G32" ]; then $(call copyfile, %s/youtube/youtube.bin , %s%s/key.bin);fi' % (_mstar_chip, config_directory, project_release_dir, _youtube_cer_path),
            'if [ ! -d %s%s ];then mkdir -p %s%s;fi' % (project_release_dir, _netflix_cer_path, project_release_dir, _netflix_cer_path),
            'mkdir -p %s/vendor/drm_data/certificates' % (project_release_dir),
            #'cp -rf %s/drm_data/certificates/*  %s/vendor/drm_data/certificates/' % (config_directory, project_release_dir),
            'mkdir -p %s/conf/EDID_BIN' % (project_release_dir),
            'cp -rf  %s/spi/edid.edid  %s/conf/EDID_BIN' % (config_directory, project_release_dir),
            'cp -rf  %s/spi/edid.vga  %s/conf/EDID_BIN' % (config_directory, project_release_dir),
    #  'if [ ! -d %s/conf/Customer ];then mkdir %s/conf/Customer;fi' % ( project_release_dir,project_release_dir),
    #  'ln -sf ../../vendor/netflix %s/conf/Customer/Netflix_env' % ( project_release_dir) ,
            '$(call copyfile, %s/edid.edid , %s/vendor/%s)' % (config_directory, project_release_dir, config_directory),
            '$(call copyfile, %s/firstswloaded.txt , %s/vendor)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/keys.bin , %s/vendor/%s)' % (config_directory, project_release_dir, config_directory),
            '$(call copyfile, %s/testtool_panel/testtool_panel.bin , %s/conf)' % (config_directory, project_release_dir),
            '$(call copyfile, profile_config/langprofile.bin , %s/vendor/%s_langprofile.bin)' % (project_release_dir, mb_name),
            '$(call copyfile, %s/VESTEL_D1_Plus_PNL.bin , %s/conf)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/VESTEL_D1_Plus_TMG.bin , %s/conf)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/df_gopc.gopc , %s/vendor)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/df_gopc_hdr.gopc , %s/vendor)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/hotel_wel.png , %s/conf)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/svn_revision_of_config.txt , %s/conf)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/TI_Peq_Settings_Mstar.cfg , %s/conf/TI_Peq_Settings.cfg)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/DTS_VirtualX_SE_SetParam.txt , %s/conf/DTS_VirtualX_SE_SetParam.txt)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/DAP_SE_SetParam.txt , %s/conf/DAP_SE_SetParam.txt)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dolby_dap_tuning_config.dat , %s/conf/dolby_dap_tuning_config.dat)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/ClickSound.pcm , %s/conf/ClickSound.pcm)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/subw_config_file.txt , %s/conf/)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/diagnostic/test_clip.mp4 , %s/conf/diagnostic)' % (config_directory, project_release_dir),
            'mkdir -p %s/%s' % (project_release_dir, _downloaded_images_dir_path),
            'if [ %d -eq 1 ]; then cp -rf %s/html_applications/%s/*  %s/vendor/html_applications/digital_ib/;fi' % ( _include_html_applications, project_root, _digital_ib_path, project_release_dir),
            'if [ %d -eq 1 ]; then mkdir %s/vendor/html_applications/tivo;fi' % ( _include_lighttpd_web_server, project_release_dir),
            'if [ %d -eq 1 ]; then tar -xf %s/html_applications/tivo/%s -C %s/vendor/html_applications/tivo/;fi' % ( _include_lighttpd_web_server, project_root, _tivo_app_package, project_release_dir),
            'if [ %d -eq 1 ]; then mkdir %s/vendor/html_applications/tivo_prestaging;fi' % ( _include_prestaging_package, project_release_dir),
            'if [ %d -eq 1 ]; then tar -xf %s/html_applications/tivo/%s -C %s/vendor/html_applications/tivo_prestaging/;fi' % ( _include_prestaging_package, project_root, _tivo_app_package_prestaging, project_release_dir),
            'if [ %d -eq 1 ]; then mkdir -p %s/vendor/lighttpd;fi' % (_include_lighttpd_web_server, project_release_dir),
            'if [ %d -eq 1 ]; then cp -rf %s/lighttpd/output/mb181/* %s/vendor/lighttpd;fi' % (_include_lighttpd_web_server, dependent_projects_path, project_release_dir),
            'if [ %d -eq 1 ]; then cp -rf  %s/3rd_party/lighttpd/lighttpd_tivo.conf %s/vendor;fi' % (_include_lighttpd_web_server, project_root, project_release_dir),
            'if [ %d -eq 1 ]; then cp -rf  %s/3rd_party/lighttpd/lighttpd_tivo_prestaging.conf %s/vendor;fi' % (_include_prestaging_package, project_root, project_release_dir),
            'if [ ! -d %s/vendor/lib ];then mkdir %s/vendor/lib;fi' % (project_release_dir,project_release_dir),
            '$(call copyfile, -f %s/mstar/bsp/%s/bin/libvdplayer.so %s/vendor/lib/)' % (ice_source_directory, _mstar_bsp, project_release_dir),
            '$(call copyfile, -f %s/mstar/bsp/%s/bin/fw* %s/vendor/config/)' % (ice_source_directory, _mstar_bsp, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-3.so.200.19.0 %s/vendor/lib/libnl-3.so.200;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-3.so.200.19.0 %s/vendor/lib/libnl-3.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-3.so.200.19.0 %s/vendor/lib/libnl.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-cli-3.so.200.19.0 %s/vendor/lib/libnl-cli-3.so.200;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-cli-3.so.200.19.0 %s/vendor/lib/libnl-cli-3.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-genl-3.so.200.19.0 %s/vendor/lib/libnl-genl-3.so.200;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-genl-3.so.200.19.0 %s/vendor/lib/libnl-genl-3.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-genl-3.so.200.19.0 %s/vendor/lib/libnl-genl.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-idiag-3.so.200.19.0 %s/vendor/lib/libnl-idiag-3.so.200;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-idiag-3.so.200.19.0 %s/vendor/lib/libnl-idiag-3.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-nf-3.so.200.19.0 %s/vendor/lib/libnl-nf-3.so.200;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-nf-3.so.200.19.0 %s/vendor/lib/libnl-nf-3.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-route-3.so.200.19.0 %s/vendor/lib/libnl-route-3.so.200;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -s ./libnl-route-3.so.200.19.0 %s/vendor/lib/libnl-route-3.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -sf ./libssl.so.1.0.0 %s/vendor/lib/libssl.so;fi' % (_include_wifi, project_release_dir),
            'if [ %d -eq 1 ]; then ln -sf ./libcrypto.so.1.0.0 %s/vendor/lib/libcrypto.so;fi' % (_include_wifi, project_release_dir),
            'cp -rf %s/SN_181/* %s/environment/release/conf/' % (config_directory, project_build_dir),
            'cp -rf %s/SN_181/* %s/environment/release/vendor/' % (config_directory, project_build_dir),
            'rm -rf %s/environment/release/vendor/Customer ' % ( project_build_dir),
            'cp -rf %s/SN_181/Customer/*.ini %s/environment/release/vendor/config/' % (config_directory, project_build_dir),
            'mkdir -p %s/conf/config/customer/' % (project_release_dir),
            'cp -rf %s/SN_181/Customer/boot.ini %s/conf/config/customer/' % (config_directory, project_release_dir),
            'cp -rf %s/SN_181/Customer/boot0.mp3 %s/conf/config/' % (config_directory, project_release_dir),
            'cp -rf %s/SN_181/Customer/boot0.jpg %s/conf/config/' % (config_directory, project_release_dir),
            'mkdir -p %s/conf/config/apollo' % (project_release_dir),
            'mkdir -p %s/conf/config/module' % (project_release_dir),
            'cp -rf %s/SN_181/Customer/boot_cfg.ini %s/conf/config/apollo' % (config_directory, project_release_dir),
            'cp -rf %s/SN_181/Customer/boot_cfg.ini %s/conf/config/module' % (config_directory, project_release_dir),
            'if [ %d -eq 1 ]; then touch %s/conf/netflix_enabled.bin;fi' % (_include_options_apm,  project_release_dir),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/brcm/bcm43236.nvm , %s/environment/release/ROOTFS/lib/firmware/brcm);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/brcm/rtecdc.bin.trx , %s/environment/release/ROOTFS/lib/firmware/brcm/bcm43236-firmware.bin);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/EEPROM_MT7663.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/EEPROM_MT7668.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/EEPROM_MT7668_e1.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/mt7668_patch_e1_hdr.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/mt7668_patch_e2_hdr.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/mt7663_patch_e2_hdr.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/TxPwrLimit_MT76x8.dat , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/TxPwrLimit_MT76x3.dat , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/wifi.cfg , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/WIFI_RAM_CODE_MT7663.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/WIFI_RAM_CODE_MT7668.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/WIFI_RAM_CODE2_USB_MT7668.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/regulatory.db , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_wifi, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/woble_setting_7668.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_bt, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/woble_setting_7663.bin , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_bt, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/mtk/bt.cfg , %s/environment/release/ROOTFS/lib/firmware/);fi' % ( _include_bt, config_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/boots , %s/environment/release/ROOTFS/bin/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/boots_srv , %s/environment/release/ROOTFS/bin/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/bt-dbg , %s/environment/release/ROOTFS/bin/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/btmw-rpc-test , %s/environment/release/ROOTFS/bin/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/btmw-test , %s/environment/release/ROOTFS/bin/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/btservice , %s/environment/release/ROOTFS/bin/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/btut , %s/environment/release/ROOTFS/bin/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/picus , %s/environment/release/ROOTFS/bin/);fi' % ( _include_bt, ice_source_directory, project_build_dir ),
            'if [ ! -d %s/conf/bluedroid ];then mkdir %s/conf/bluedroid;fi' % ( project_release_dir, project_release_dir),
            'if [ ! -d %s/conf/bluetooth ];then mkdir %s/conf/bluetooth;fi' % ( project_release_dir, project_release_dir),
            'if [ ! -d %s/conf/bluetooth/logs ];then mkdir %s/conf/bluetooth/logs;fi' % ( project_release_dir, project_release_dir),
            'if [ %d -eq 1 ]; then $(call copyfile, %s/bluetooth/mtk/mt7668/arm-linux-gnueabi_5.5.0/bin/bt*.conf , %s/conf/bluedroid/);fi' % ( _include_bt, ice_source_directory, project_release_dir ),
            'cp -f %s/environment/distribution/vendor/scripts/default.script %s/ROOTFS/etc/udhcpc/' % (project_build_dir, project_release_dir),
            'cp -f %s/environment/distribution/vendor/scripts/zcip.script %s/ROOTFS/etc/udhcpc/' % (project_build_dir, project_release_dir),
            'mkdir -p %s/ROOTFS/vendor_config/' % (project_release_dir),
            'cp -rf %s/SN_181/Customer/*.ini %s/environment/release/ROOTFS/vendor_config/' % (config_directory, project_build_dir),
            'cp -rf %s/SN_181/config/* %s/environment/release/ROOTFS/vendor_config/' % (config_directory, project_build_dir),
            'cp -rf %s/environment/distribution/vendor/config/environment_%s.txt %s/vendor/config/environment.txt' % (project_build_dir, _mmap_config, project_release_dir),
            "sed -i 's|\/config\/|\/vendor_config\/|g'  %s/ROOTFS/vendor_config/sys.ini" % (project_release_dir),
            "sed -i 's|\/config\/|\/vendor_config\/|g'  %s/ROOTFS/vendor_config/model/*" % (project_release_dir),
            'if [ %d -eq 1 ] ; then tar -xzf %s/environment/distribution/LINUX_ROOTFS/zoneinfo.tar.gz -C %s/ROOTFS/usr/share/;fi' % ( _connected,  project_build_dir, project_release_dir ),
            #'ln -sT /conf/PM.bin %s/vendor/PM.bin' % ( project_release_dir ),
            'cp -rf %s/environment/os/pm/%s/PM.bin %s/vendor/' % (project_build_dir, _mstar_chip, project_release_dir),
            'cp -rf %s/environment/os/pm/%s/RT_PM.bin %s/vendor/' % (project_build_dir, _mstar_chip, project_release_dir),
            ## Collect files for CUSSET
            'mkdir -p %s/CUSSET' % (project_release_dir),
            'cp -f %s/vendor/config/environment.txt %s/CUSSET' % (project_release_dir, project_release_dir),
            'cp -f %s/vendor/RT_PM.bin %s/CUSSET' % (project_release_dir, project_release_dir),
            'cp -f %s/vendor/config/mmap.ini %s/CUSSET' % (project_release_dir, project_release_dir),
            "touch %s/conf/msyslog_enable.bin" % (project_release_dir),
            "echo 999 >> %s/conf/always_logging_reset_limit" % (project_release_dir),
            "echo 0 >> %s/conf/always_logging_reset_count" % (project_release_dir),
            "echo 999 >> %s/conf/always_logging_day_limit" % (project_release_dir),
            "echo 0 >> %s/conf/always_logging_day_count" % (project_release_dir),
            'touch %s/conf/project.%s.%s_%d.%d' % (project_release_dir, mb_name_sw_config_tool, _storage, _storage_size, _apv_v5),
             # Un-comment next two lines to re-locate hosts file to rw partition and add a symbolic link to allow updating without re-flashing
             #'mv -f %s/ROOTFS/etc/hosts %s/conf/' % (project_release_dir, project_release_dir),
             #'ln -sT /conf/hosts %s/ROOTFS/etc/hosts' % (project_release_dir),
             #------ START For wifi debug, move below files to /conf
             #"mv -f %s/ROOTFS/usr/bin/wpa_cli %s/conf/" % (project_release_dir, project_release_dir),
             #"mv -f %s/ROOTFS/usr/bin/wpa_passphrase %s/conf/" % (project_release_dir, project_release_dir),
             #"mv -f %s/ROOTFS/usr/bin/wpa_supplicant %s/conf/" % (project_release_dir, project_release_dir),
             #"ln -sT /conf/wpa_cli %s/ROOTFS/usr/bin/wpa_cli" % (project_release_dir),
             #"ln -sT /conf/wpa_passphrase %s/ROOTFS/usr/bin/wpa_passphrase" % (project_release_dir),
             #"ln -sT /conf/wpa_supplicant %s/ROOTFS/usr/bin/wpa_supplicant" % (project_release_dir),
             #"mv -f %s/ROOTFS/lib/modules/wlan_mt7668_usb.ko %s/conf/" % (project_release_dir, project_release_dir),
             #"ln -sT /conf/wlan_mt7668_usb.ko %s/ROOTFS/lib/modules/wlan_mt7668_usb.ko" % (project_release_dir),
             #"mv -f %s/ROOTFS/lib/firmware %s/conf/" % (project_release_dir, project_release_dir),
             #"ln -sT /conf/firmware %s/ROOTFS/lib/firmware" % (project_release_dir),
             #------ END For wifi debug, move below files to /conf
             'ln -sf ../../conf/directfbrc %s/vendor/config/directfbrc' % (project_release_dir),
            ]

    if include_options.g31:
        if include_options.vewd_sdk423 and not include_options.uk2023_legacy_projects:
            # for Vewd 4.23 G31 TiVo project
            vendor_commands += [
            'ln -sf ../../conf/directfbrc %s/vendor/config/directfbrc' % (project_release_dir),
            'ln -sf ../../conf/directfbrc_OLED %s/vendor/config/directfbrc_OLED' % (project_release_dir),
            '$(call copyfile, %s/youtube/g31-mb180_youtube_tivo.bin , %s%s/key.bin)' % (config_directory, project_release_dir, _youtube_cer_path),
            'echo -e \"\033[36m tivo youtube key is copied \033[0m\" ',
            ]
        else:
            # QUIGON for G31
            vendor_commands += [
            'ln -sf ../../conf/directfbrc %s/vendor/config/directfbrc' % (project_release_dir),
            'ln -sf ../../conf/directfbrc_OLED %s/vendor/config/directfbrc_OLED' % (project_release_dir),
            '$(call copyfile, %s/youtube/g31-mb180_youtube.bin , %s%s/key.bin)' % (config_directory, project_release_dir, _youtube_cer_path),
            'echo -e \"\033[36m quigon youtube key is copied \033[0m\" ',
            ]

    if include_options.vewd_sdk423 and not include_options.uk2023_legacy_projects:
        # for Vewd 4.23 G31/G32 TiVo project
        vendor_commands += [
        'cp -rf %s/oss/tivo/oss.txt %s/vendor/oss' % (config_directory, project_release_dir),
        ]
    else:
        # MB180 (QuiGon and ObiWan)
        vendor_commands += [
        'cp -rf %s/oss/oss.txt %s/vendor/oss' % (config_directory, project_release_dir),
        ]

    if include_options.nand512:
            vendor_commands += [
            'cd %s/mstar/oadota/;./build.sh %s' % (ice_source_directory, TOOLCHAIN_PATH),
            '$(call copyfile, %s/mstar/oadota/oadota_proc , %s/vendor/)' % (ice_source_directory, project_release_dir),
            '$(call copyfile, %s/mstar/oadota/nandwrite , %s/vendor)' % (ice_source_directory, project_release_dir),
            '$(call copyfile, %s/mstar/oadota/flash_erase , %s/vendor)' % (ice_source_directory, project_release_dir),
            '$(call copyfile, %s/mstar/oadota/mtd_debug , %s/vendor)' % (ice_source_directory, project_release_dir),
            ]

    # If profilemerger will be used, copy the binaries created by profilemerger
    # instead of those in the profile_config directory to the project_release_dir.

    _include_base_profile = 0
    _include_merge_profile = 0
    
    mstar_default_profile_path = 'profile_config/mb180'
    mstar_base_profile_file_path = 'profile_config/mb180'
    if _include_hotel_tv:
        default_profile_path = 'profile_config/mb180/mb180_hoteltv'
        base_profile_file_path = 'profile_config/mb180/mb180_hoteltv'
    else:
        default_profile_path = 'profile_config/mb180'
        base_profile_file_path = 'profile_config/mb180'
        
    if get_profile():
        _include_merge_profile = 1
        custom_profile_file = os.path.basename(get_profile())
        custom_profile_name = custom_profile_file.rsplit('.', 1)[0]
        profile_merger_output_dir = os.path.join('$(derived_dir)', 'profiles', custom_profile_name)

        if get_profile_output():
            profile_merger_output_dir = os.path.join(get_profile_output(), custom_profile_name)

        base_profile_file_path = profile_merger_output_dir

        profiles = ['mb180_swprofile.bin', 'mb180_hwprofile.bin']

        for profile in profiles:
            vendor_commands.append('$(call copyfile, {} , {})'.format(
                os.path.join(profile_merger_output_dir, profile), os.path.join(project_release_dir, 'conf', profile)))
    else:
        if get_base_profile():
            _include_base_profile = 1
            base_profile_file_path = os.path.split(get_base_profile())[0]
            vendor_commands.append('$(call copyfile, %s/mb180_swprofile.bin , %s/conf/mb180_swprofile.bin)' % (base_profile_file_path, project_release_dir))
            vendor_commands.append('$(call copyfile, %s/mb180_hwprofile.bin , %s/conf/mb180_hwprofile.bin)' % (base_profile_file_path, project_release_dir))
        else:
            vendor_commands.append('$(call copyfile, %s/mb180_swprofile.bin , %s/conf/mb180_swprofile.bin)' % (default_profile_path, project_release_dir))
            vendor_commands.append('$(call copyfile, %s/mb180_hwprofile.bin , %s/conf/mb180_hwprofile.bin)' % (default_profile_path, project_release_dir))

    # If the executable is built with debug, specify that core files should be
    # dumped if it crashes.

   # if 'D' in canonical_variant:
        #vendor_commands.append('touch {0}/conf/dump_enabled.bin'.format(project_release_dir))
    if _include_AMZ_support:
        if include_options.g32:
            vendor_commands.append('if [ ! -d %s%s ];then mkdir -p %s%s;fi' % (project_release_dir, _amazon_cacert_path, project_release_dir, _amazon_cacert_path))
            vendor_commands.append('$(call copyfile, %s/mstar/amazon/%s/cacerts.pem , %s%s/cacerts.pem)' % (ice_source_directory, _mstar_chip, project_release_dir, _amazon_cacert_path))

    special_build(
        'vendor',
        command = tuple(vendor_commands),
        phony = True,
        requires = 'elf get_ca_bundle',
    )

    if include_options.environment_generator:
        special_build(
                'bin',
                command = (
                    'cd %s/environment/mscript; ./usb_image_build.sh noSPI' % (project_build_dir),
                    'cd %s/environment/mscript; ./usb_image_build.sh no_conf' % (project_build_dir),
                    '%s/scripts/make_environment_generator_delivery.sh %s %s %s %s ' % (project_build_dir, project_directory, _ui_software_version, mb_name, connected_str),
                ),
                phony = True,
                requires = _link_requires,
        )
    else:
        special_build(
                'bin',
                command = (
                    'cd %s/environment/mscript; ./usb_image_build.sh noSPI' % (project_build_dir),
                ),
                phony = True,
                requires = _link_requires,
        )

#    if include_options.bluetooth:
#        special_build(
#        'copy_bluetooth_server',
#            command = ( '$(call copyfile, %s/bluetooth/brcm/bsa_server , %s/vendor)' % (ice_source_directory, project_release_dir),
#                        '$(call copyfile, %s/bluetooth/brcm/fw/BCM20702B0_002.001.014.0505.0619.hcd , %s/vendor)' % (ice_source_directory, project_release_dir),
#                      ),
#            )

if include_options.dynamic_ui and 'createDist' not in options.targets:
    special_build(
    'build_dynamic_ui',
    command = ('if [ ! -d $(derived_dir)/%s_ui ];then mkdir $(derived_dir)/%s_ui;fi' % (_dynamic_ui_name, _dynamic_ui_name),
               'rm -rf $(derived_dir)/%s_ui/*'% (_dynamic_ui_name),
               'echo %s_svg.txt>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s_svg.bin>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s_img.txt>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s_img.bin>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s.img>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s_font_byte_data.txt>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s_font_byte_data.bin>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s_font_file_data.txt>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name,  _dynamic_ui_name),
               'echo %s_font_file_data.bin>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s>>$(derived_dir)/%s_ui/%s.ui' % (_ui_software_version, _dynamic_ui_name, _dynamic_ui_name),
               'echo %s>>$(derived_dir)/%s_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name, _dynamic_ui_name),
               'cp -f %s%s%s %s%s%s%s%s' % ("$(derived_dir)/", _design_folder, "/default_svg.txt", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, "_svg.txt"),
               'cp -f %s%s%s %s%s%s%s%s' % ("$(derived_dir)/", _design_folder, "/default_svg.bin", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, "_svg.bin"),
               'cp -f %s%s %s%s%s%s%s' % ("$(derived_dir)/app_orion/", "default_img.txt", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, "_img.txt"),
               'cp -f %s%s %s%s%s%s%s' % ("$(derived_dir)/app_orion/", "default_img.bin", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, "_img.bin"),
               'cp -f %s%s%s%s %s%s%s%s%s' % (_design_folder, "/png/", _design_name, ".img", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, ".img"),
               'cp -f %s%s %s%s%s%s%s' % ("$(derived_dir)/app_orion/", "default_font_byte_data.txt", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, "_font_byte_data.txt"),
               'cp -f %s%s %s%s%s%s%s' % ("$(derived_dir)/app_orion/", "default_font_byte_data.bin", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, "_font_byte_data.bin"),
               'cp -f %s%s %s%s%s%s%s' % ("$(derived_dir)/app_orion/", "default_font_file_data.txt", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, "_font_file_data.txt"),
               'cp -f %s%s %s%s%s%s%s' % ("$(derived_dir)/app_orion/", "default_font_file_data.bin", "$(derived_dir)/", _dynamic_ui_name, "_ui/", _dynamic_ui_name, "_font_file_data.bin"),
              )
    )

if 'build_dynamic_ui' not in options.targets and 'createDist' not in options.targets:
    _debug = 'debug' if 'D' in canonical_variant else 'no_debug'

    if not mstar_user_build and not vewd_user_build and not amazon_user_build:
        if include_options.dynamic_ui:
            special_build(
                'copy_default_ui',
                command = ('if [ ! -d $(derived_dir)/dynamic_ui ];then mkdir $(derived_dir)/dynamic_ui;fi',
                           'rm -rf $(derived_dir)/dynamic_ui/*',
                           'echo %s_svg.txt>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s_svg.bin>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s_img.txt>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s_img.bin>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s.img>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s_font_byte_data.txt>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s_font_byte_data.bin>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s_font_file_data.txt>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s_font_file_data.bin>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'echo %s>>$(derived_dir)/dynamic_ui/%s.ui' % (_ui_software_version, _dynamic_ui_name),
                           'echo %s>>$(derived_dir)/dynamic_ui/%s.ui' % (_dynamic_ui_name, _dynamic_ui_name),
                           'cp -f %s%s%s %s%s%s' % ("$(derived_dir)/", _design_folder, "/default_svg.txt", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, "_svg.txt"),
                           'cp -f %s%s%s %s%s%s' % ("$(derived_dir)/", _design_folder, "/default_svg.bin", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, "_svg.bin"),
                           'cp -f %s%s %s%s%s' % ("$(derived_dir)/app_orion/", "default_img.txt", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, "_img.txt"),
                           'cp -f %s%s %s%s%s' % ("$(derived_dir)/app_orion/", "default_img.bin", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, "_img.bin"),
                           'cp -f %s%s%s%s %s%s%s' % (_design_folder, "/png/", _design_name, ".img", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, ".img"),
                           'cp -f %s%s %s%s%s' % ("$(derived_dir)/app_orion/", "default_font_byte_data.txt", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, "_font_byte_data.txt"),
                           'cp -f %s%s %s%s%s' % ("$(derived_dir)/app_orion/", "default_font_byte_data.bin", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, "_font_byte_data.bin"),
                           'cp -f %s%s %s%s%s' % ("$(derived_dir)/app_orion/", "default_font_file_data.txt", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, "_font_file_data.txt"),
                           'cp -f %s%s %s%s%s' % ("$(derived_dir)/app_orion/", "default_font_file_data.bin", "$(derived_dir)/dynamic_ui/", _dynamic_ui_name, "_font_file_data.bin"),
                           'if [ ! -d %s/vendor/dynamic_ui/ ];then mkdir %s/vendor/dynamic_ui/; fi' %(project_release_dir, project_release_dir),
                           'rm -rf %s/vendor/dynamic_ui/*'%(project_release_dir),
                           'cp -rf $(derived_dir)/dynamic_ui/*.bin %s/vendor/dynamic_ui/'%(project_release_dir),
                           'cp -rf $(derived_dir)/dynamic_ui/*.txt %s/vendor/dynamic_ui/'%(project_release_dir),
                           'cp -rf $(derived_dir)/dynamic_ui/*.img %s/vendor/dynamic_ui/'%(project_release_dir),
                           'cp -rf $(derived_dir)/dynamic_ui/%s.ui %s/vendor/dynamic_ui/default.ui' % (_dynamic_ui_name, project_release_dir),
                           '%s -o %s/vendor/aurora.elf $(derived_dir)/aurora.elf' % (strip, project_release_dir),
                           '%s/make_vendor.sh %s %s %s %s %s' % (project_build_dir, project_directory, nfsbuild, _debug, ice_source_directory, _include_AMZ_support),
                           'if [ %d -eq 0 ]; then cd %s/environment; make partition-all;fi' % (_enable_nfsbuild, project_build_dir),
                          ),
                requires = 'vendor copy_dmr_files',
            )
        else:
            special_build(
                'copy_default_ui',
                command = (
                '%s -o %s/vendor/aurora.elf $(derived_dir)/aurora.elf' % (strip, project_release_dir),
                '%s/make_vendor.sh %s %s %s %s %s' % (project_build_dir, project_directory, nfsbuild, _debug, ice_source_directory, _include_AMZ_support),
                'cd %s/environment; make partition-all' % (project_build_dir),
                ),
                requires = 'vendor copy_dmr_files',
            )
    else:
        if include_options.dynamic_ui:
            special_build(
                'copy_default_ui',
                command = ('if [ ! -d %s/vendor/dynamic_ui/ ];then mkdir %s/vendor/dynamic_ui/; fi' %(project_release_dir,project_release_dir),
                       'rm -rf %s/vendor/dynamic_ui/*'%(project_release_dir),
                       'cp -rf %s/external_delivery/dynamic_ui/*.bin %s/vendor/dynamic_ui/'%(project_build_dir, project_release_dir),
                       'cp -rf %s/external_delivery/dynamic_ui/*.txt %s/vendor/dynamic_ui/'%(project_build_dir, project_release_dir),
                       'cp -rf %s/external_delivery/dynamic_ui/*.img %s/vendor/dynamic_ui/'%(project_build_dir, project_release_dir),
                       'cp -rf %s/external_delivery/dynamic_ui/default.ui %s/vendor/dynamic_ui/default.ui' %(project_build_dir, project_release_dir),
                       '%s/make_vendor.sh %s %s %s %s %s' % (project_build_dir, project_directory, nfsbuild, _debug, ice_source_directory, _include_AMZ_support),
                       'cd %s/environment; make partition-all' % (project_build_dir),
                      ),
                requires = 'vendor copy_dmr_files',
            )
        else:
            special_build(
                'copy_default_ui',
                command = (
                        '%s/make_vendor.sh %s %s %s %s %s' % (project_build_dir, project_directory, nfsbuild, _debug, ice_source_directory, _include_AMZ_support),
                        'cd %s/environment; make partition-all' % (project_build_dir),
                ),
                requires = 'vendor copy_dmr_files',
            )

    special_build(
        'copy_dmr_files',
        command = (
            '$(call copyfile, %s/dlna_dmr/dmr.xml , %s/conf/dlna-dmr/xml)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/dmr_avts.xml , %s/conf/dlna-dmr/xml)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/dmr_cms.xml , %s/conf/dlna-dmr/xml)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/dmr_rcs.xml , %s/conf/dlna-dmr/xml)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/dmr_desc.xml , %s/conf/dlna-dmr/xml)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/sink_proto_info.xml , %s/conf/dlna-dmr/xml)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/icon/DMR-large.jpg , %s/conf/dlna-dmr/xml/icon)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/icon/DMR-large.png , %s/conf/dlna-dmr/xml/icon)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/icon/DMR-small.jpg , %s/conf/dlna-dmr/xml/icon)' % (config_directory, project_release_dir),
            '$(call copyfile, %s/dlna_dmr/icon/DMR-small.png , %s/conf/dlna-dmr/xml/icon)' % (config_directory, project_release_dir),
        ),
        requires = 'vendor',
    )

    special_build(
        'copy_dms_files',
        command = (
            '$(call copyfile, %s/dlna_dms/access_conf.xml , %s/conf/dlna-dmr/xml)' % (config_directory, project_release_dir),
        ),
        requires = 'vendor copy_dmr_files',
    )

    special_build(
        'copy_application_scripts',
        command = (
            'cp -rf %s/environment/distribution/vendor/scripts/* %s/vendor/scripts/'%(project_build_dir, project_release_dir),
            'rm %s/vendor/scripts/.svn -rf'%(project_release_dir),
        ),
        requires = 'vendor',
    )

    #def create_nfs_scripts():
    #    sys.path.append(os.path.join(project_directory, 'mb130_tools'))
    #    return_cmd = str()
    #    try:
    #        from nfs_script_maker import create_scripts
    #        nfs_enabled = 'nfs' in options.targets
    #        return_cmd = create_scripts(project_root, project_release_dir, nfs_enabled)
    #    except ImportError, e:
    #        return_cmd = 'echo conf.py unable to generate scripts'
    #    return return_cmd

    #special_build(
    #    'create_debug_scripts',
    #    command = (
    #        create_nfs_scripts(),
    #    ),
    #)

    if os.geteuid() != 0:
        special_build(
            'nfs',
            command = (
            'mkdir -p nfsroot',
            'fakeroot cp -rf %s/ROOTFS/* nfsroot' % project_release_dir,
            'mkdir -p nfsroot/conf',
            'mkdir -p nfsroot/vendor',
            'fakeroot cp %s/conf/* nfsroot/conf/ -rf' % project_release_dir,
            'fakeroot cp %s/vendor/* nfsroot/vendor/ -rf' % project_release_dir,
            'fakeroot %s -o nfsroot/vendor/aurora.elf $(derived_dir)/aurora.elf' % (strip,),
            "cp %s/environment/toolbox/arm-gdb %s/vendor/" % (project_build_dir, project_release_dir),
            ),
            phony = True,
            requires = _link_requires,
        )
    else:
        special_build(
            'nfs',
            command = (
            'mkdir -p nfsroot',
            'cp -rf %s/ROOTFS/* nfsroot' % project_release_dir,
            'mkdir -p nfsroot/conf',
            'mkdir -p nfsroot/vendor',
            'cp %s/conf/* nfsroot/conf/ -rf' % project_release_dir,
            'cp %s/vendor/* nfsroot/vendor/ -rf' % project_release_dir,
            '%s -o nfsroot/vendor/aurora.elf $(derived_dir)/aurora.elf' % (strip,),
            ),
            phony = True,
            requires = _link_requires,
        )

    def create_external_delivery_jsplugin_command(jsplugins):
        """Create the jsplugin command for external delivery.
        
        Create the commands to copy the static libraries required to build the plugin
        dynamic libraries for external delivery.
        The generated command:
            - Creates any required directories within $(derived_dir)/external_delivery.
            - Copies all static libraries from their built location to external_delivery.
            - Adds an entry to copy_folders.sh.

        Parameters:
            jsplugins - A list of plugin paths.
                (e.g. 'vewd_common/jsplugins/profile_status')
        
        Returns:
            A tuple of commands.

        """

        external_delivery = os.path.join('$(derived_dir)', 'external_delivery')
        plugin_external_delivery = os.path.join(external_delivery, EXTERNAL_DELIVERY_PLUGINS_DIR)

        def copy_command(src, dest):
            return ('mkdir -p {};'.format(dest),
                    'cp -r {} {};'.format(src, dest))

        jsplugins_command = ()

        for plugin in jsplugins:
            plugin_dir = subfolder(plugin)
            plugin_src = os.path.join('$(derived_dir)', plugin, libname(plugin_dir))
            plugin_dest = os.path.join(plugin_external_delivery, plugin_dir)
            jsplugins_command += copy_command(plugin_src, plugin_dest)

        jsplugins_command += ('echo "cp -rv {}/ ../../../">>{};'.format(
                              EXTERNAL_DELIVERY_PLUGINS_DIR, 
                              os.path.join(external_delivery, 'copy_folders.sh')),)

        return jsplugins_command

    def create_external_delivery_copy_ipc_command():
        ''' Returns a tuple of command strings to copy ipc related archives required for the
            external delivery.
        '''
        cmd = (
            'if [ %s = "True" ];then echo "cp -rv 3rd_party_wrapper/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("3rd_party_wrapper/rpclib")),
            'mkdir -p $(derived_dir)/external_delivery/project/rpclib/output/;',
            'cp -r project/rpclib/output/mb181 $(derived_dir)/external_delivery/project/rpclib/output/;',
            'mkdir -p $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/$(call libname,ipc) $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/shared/frost_src/$(call libname,frost_src) $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/shared/http_client_src/$(call libname,http_client_src) $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/shared/language_src/$(call libname,language_src) $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/shared/nebula_src/$(call libname,nebula_src) $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/shared/utilities_public_src/$(call libname,utilities_public_src) $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/shared/utilities_private_src/$(call libname,utilities_private_src) $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/shared/utilities_debug_src/$(call libname,utilities_debug_src) $(derived_dir)/external_delivery/libs_mumin/;',
            'cp -r $(derived_dir)/vewd_integration/ipc/shared/vewd_integration/$(call libname,vewd_integration) $(derived_dir)/external_delivery/libs_mumin/;',
            'echo "cp -rv libs_mumin/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;',
            )
        return cmd

    def create_external_delivery_copy_drmbackend_command():
        ''' Returns a tuple of command strings to copy drm backend (ciplus) related archives required for the
            external delivery.
        '''
        cmd = (
            'mkdir -p $(derived_dir)/external_delivery/libs_drm_backends/;',
            'cp -r $(derived_dir)/vewd_integration/drm_backends/ciplus/$(call libname,ciplus) $(derived_dir)/external_delivery/libs_drm_backends/;',
            'echo "cp -rv libs_drm_backends/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;',
            )
        return cmd

    def create_external_delivery_copy_for_vewdlib_command():
        cmd = (
            'touch $(derived_dir)/../scripts/vewd_extra_libs_temp.txt;',
            'if [ %d -eq 1 ];then echo "addlib aurora_trunk_dtv/mb180/derived.vewd.T/vewd_integration/libvewd_integration.a">> $(derived_dir)/../scripts/vewd_extra_libs_temp.txt ;fi' % (_include_sdk420),
            'chmod 777 -R $(derived_dir)/../scripts/vewd_extra_libs.txt',
            'cat $(derived_dir)/../scripts/vewd_extra_libs.txt >> $(derived_dir)/../scripts/vewd_extra_libs_temp.txt',
            )
        return cmd   
         
    vewdlib_command = create_external_delivery_copy_for_vewdlib_command() 
    vewdlib_command += ('mkdir -p $(derived_dir)/external_delivery/',
        # make aurora_lib for vewd. Copy library and informative scripts.
        "sed -i '/save/d' %s;cat $(derived_dir)/../scripts/vewd_extra_libs_temp.txt >> %s;cat %s;$(call mklib,%s );" % (libs_txt,libs_txt,libs_txt,libs_txt),
        "rm -rf $(derived_dir)/../scripts/vewd_extra_libs_temp.txt",
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (libs_txt),
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (aurora_lib),
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (no_access_txt),

        # copy various extra files
        'if [ %s = "True" ];then mkdir -p $(derived_dir)/external_delivery/svg_components;fi'% (is_component_used("svg_components")),
        'if [ %s = "True" ];then cp svg_components/* $(derived_dir)/external_delivery/svg_components/;fi'% (is_component_used("svg_components")),

        'mkdir -p $(derived_dir)/external_delivery/build_info',
        'cp $(derived_dir)/build_info/* $(derived_dir)/external_delivery/build_info/',

        'mkdir -p $(derived_dir)/external_delivery/certificates',
        'cp certificates/cacert.pem $(derived_dir)/external_delivery/certificates/cacert.pem',
        'cp certificates/prod-partner-netflix-net-chain.pem $(derived_dir)/external_delivery/certificates/prod-partner-netflix-net-chain.pem',

        # copy files indicated by forbidden dist.
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/DvbDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("dvb")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/EclipseSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("eclipse_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/EclipseDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("eclipse")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/FVCDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("fvc")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/NebulaSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("nebula_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/NovoDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("novo")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/IncaDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("inca")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/LibdlnaDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("libdlna_access_3.0")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/LynxDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("lynx")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/MplibDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("3rd_party/mplib-1.0.3")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/Sqlite3WrapperDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("3rd_party_wrapper/sqlite3")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/TinyxmlWrapperDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("3rd_party_wrapper/tinyxml")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/UtilitiesPrivateSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("utilities_private_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/UtilitiesPublicSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("utilities_public_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/LanguageDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("language")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/HttpClientDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("http_client")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/CurlWrapperDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("3rd_party_wrapper/curl")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/RpclibWrapperDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("3rd_party_wrapper/rpclib")),

        # copy dependent project includes and libs - not static; should be copied instead of adding into mstar_extra_libs.txt
        'mkdir -p $(derived_dir)/external_delivery/project/c-ares/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/libcurl/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/libxml2/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/openssl/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/live555/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/zlib/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/libpng/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/freetype/output/;',
        'cp -r project/c-ares/output/mb181 $(derived_dir)/external_delivery/project/c-ares/output/;',
        'cp -r project/libcurl/output/mb181 $(derived_dir)/external_delivery/project/libcurl/output/;',
        'cp -r project/libxml2/output/mb181 $(derived_dir)/external_delivery/project/libxml2/output/;',
        'cp -r project/openssl/output/mb181 $(derived_dir)/external_delivery/project/openssl/output/;',
        'cp -r project/live555/output/mb181 $(derived_dir)/external_delivery/project/live555/output/;',
        'cp -r project/zlib/output/mb181 $(derived_dir)/external_delivery/project/zlib/output/;',
        'cp -r project/libpng/output/mb181 $(derived_dir)/external_delivery/project/libpng/output/;',
        'cp -r project/freetype/output/mb181 $(derived_dir)/external_delivery/project/freetype/output/;',

        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/nghttp2/output/;fi' % (_include_options_nghttp2),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/sqlite/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/ffmpeg/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/portaudio/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/webvtt/output/;fi' % (_avs_caption_support),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/avs/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/libwebsockets/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/asio/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/websocketpp/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/apl/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/mmsdk/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/aws_iot/output/;fi' % (_include_options_aws_iot),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/pubsub/;fi' % (_include_options_extended_logging),
        'if [ %d -eq 1 ]; then cp -r project/nghttp2/output/mb181 $(derived_dir)/external_delivery/project/nghttp2/output/;fi' % (_include_options_nghttp2),
        'if [ %d -eq 1 ]; then cp -r project/sqlite/output/mb181 $(derived_dir)/external_delivery/project/sqlite/output/;fi' % (_include_options_avs),
        'if [ {} -eq 1 ]; then cp -r project/ffmpeg/output/mb181 $(derived_dir)/external_delivery/project/ffmpeg/output/;fi;'.format(_include_options_avs),
        'if [ %d -eq 1 ]; then cp -r project/portaudio/output/mb181 $(derived_dir)/external_delivery/project/portaudio/output/;fi' % (_include_options_avs),
        'if [ {} -eq 1 ]; then cp -r project/webvtt/output/mb181 $(derived_dir)/external_delivery/project/webvtt/output/;fi'.format(_avs_caption_support),
        'if [ {} -eq 1 ]; then cp -r project/avs/output/mb181 $(derived_dir)/external_delivery/project/avs/output/;fi'.format(_include_options_avs),
        'if [ {} -eq 1 ]; then cp -r project/libwebsockets/output/mb181 $(derived_dir)/external_delivery/project/libwebsockets/output/;fi'.format(_include_options_avs),
        'if [ %d -eq 1 ]; then cp -r project/asio/output/mb181 $(derived_dir)/external_delivery/project/asio/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then cp -r project/websocketpp/output/mb181 $(derived_dir)/external_delivery/project/websocketpp/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then cp -r project/apl/output/mb181 $(derived_dir)/external_delivery/project/apl/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then cp -r project/mmsdk/output/mb181 $(derived_dir)/external_delivery/project/mmsdk/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then cp -r project/aws_iot/output/mb181 $(derived_dir)/external_delivery/project/aws_iot/output/;fi' % (_include_options_aws_iot),
        'if [ %d -eq 1 ]; then cp -r project/pubsub/* $(derived_dir)/external_delivery/project/pubsub;fi' % (_include_options_extended_logging),

        # We will deliver above files to external users. The script generated below copies these files to their own working copy.
        'echo "#!/bin/bash">$(derived_dir)/external_delivery/copy_folders.sh',
        'echo "cp -rv build_info/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh',
        'echo "cp -rv certificates/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh',
        'if [ %s = "True" ];then echo "cp -rv dvb/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("dvb")),
        'if [ %s = "True" ];then echo "cp -rv eclipse/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("eclipse")),
        'if [ %s = "True" ];then echo "cp -rv eclipse_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("eclipse_src")),
        'if [ %s = "True" ];then echo "cp -rv fvc/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("fvc")),
        'if [ %s = "True" ];then echo "cp -rv inca/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("inca")),
        'if [ %s = "True" ];then echo "cp -rv libdlna_access_3.0/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("libdlna_access_3.0")),
        'if [ %s = "True" ];then echo "cp -rv lynx/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("lynx")),
        'if [ %s = "True" ];then echo "cp -rv mplib-1.0.3/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("mplib-1.0.3")),
        'if [ %s = "True" ];then echo "cp -rv nebula_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("nebula_src")),
        'if [ %s = "True" ];then echo "cp -rv novo/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("novo")),
        'if [ %s = "True" ];then echo "cp -rv svg_components/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("svg_components")),
        'if [ %s = "True" ];then echo "cp -rv utilities_private_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("utilities_private_src")),
        'if [ %s = "True" ];then echo "cp -rv utilities_public_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("utilities_public_src")),
        'if [ %s = "True" ];then echo "cp -rv language/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("language")),
        'if [ %s = "True" ];then echo "cp -rv http_client/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("http_client")),
        'if [ %s = "True" ];then echo "cp -rv 3rd_party_wrapper/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%
            (is_component_used("3rd_party_wrapper/curl") or
             is_component_used("3rd_party_wrapper/sqlite3") or
             is_component_used("3rd_party_wrapper/tinyxml") or
             (include_options.vewd_sdk423 and is_component_used("3rd_party_wrapper/rpclib"))),
        'echo "cp -rv project/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;',
    )

    if include_options.dynamic_ui:
        vewdlib_command += (
        'if [ ! -d $(derived_dir)/external_delivery/dynamic_ui ];then mkdir $(derived_dir)/external_delivery/dynamic_ui;fi',
        'cp -rf %s/environment/release/vendor/dynamic_ui/* $(derived_dir)/external_delivery/dynamic_ui/' % (project_build_dir),
    )

    if include_options.vewd_sdk423:
        vewdlib_command += create_external_delivery_copy_ipc_command()
        vewdlib_command += create_external_delivery_copy_drmbackend_command()
        vewdlib_command += create_external_delivery_jsplugin_command(jsplugins)

    if _include_lighttpd_web_server:
        vewdlib_command += (
            'mkdir -p $(derived_dir)/external_delivery/project/lighttpd/output/;',
            'cp -r project/lighttpd/output/mb181 $(derived_dir)/external_delivery/project/lighttpd/output/;',
            'mkdir -p $(derived_dir)/external_delivery/html_applications/tivo/;',
            'cp -r %s/html_applications/tivo/%s $(derived_dir)/external_delivery/html_applications/tivo/;' % (project_root, _tivo_app_package),
            'if [ %d -eq 1 ];then cp -r %s/html_applications/tivo/%s $(derived_dir)/external_delivery/html_applications/tivo/;fi' % (_include_prestaging_package, project_root, _tivo_app_package_prestaging),
            'echo "cp -rv html_applications/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;',
         )

    vewdlib_command +=(
        # make copy script executable.
        'chmod +x $(derived_dir)/external_delivery/copy_folders.sh',
        # give write permission to files in external_delivery
        'chmod -R +w $(derived_dir)/external_delivery',

        # remove unnecessary files
        '$(call rmfile,%s)' %(aurora_lib),
        '$(call rmfile,%s)' %(no_access_txt),
        '$(call rmfile,%s)' %(libs_txt),
        )

    mstarlib_command = ('mkdir -p $(derived_dir)/external_delivery/',
        "sed -i '/save/d' %s;cat $(derived_dir)/../scripts/mstar_extra_libs%s.txt >> %s;cat %s;$(call mklib,%s );" % (libs_txt,_bin_file_name_option,libs_txt,libs_txt,libs_txt),
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (libs_txt),
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (aurora_lib),
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (no_access_txt),

        'if [ %s = "True" ];then mkdir -p $(derived_dir)/external_delivery/svg_components;fi'% (is_component_used("svg_components")),
        'if [ %s = "True" ];then cp svg_components/* $(derived_dir)/external_delivery/svg_components/;fi'% (is_component_used("svg_components")),

        'mkdir -p $(derived_dir)/external_delivery/build_info',
        'cp $(derived_dir)/build_info/* $(derived_dir)/external_delivery/build_info/',

        'mkdir -p $(derived_dir)/external_delivery/certificates',
        'cp certificates/cacert.pem $(derived_dir)/external_delivery/certificates/cacert.pem',
        'cp certificates/prod-partner-netflix-net-chain.pem $(derived_dir)/external_delivery/certificates/prod-partner-netflix-net-chain.pem',

        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/EclipseSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("eclipse_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/IncaDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("inca")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/LibdlnaDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("libdlna_access_3.0")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/LynxDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("lynx")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/MplibDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("mplib-1.0.3")),
        'if [ %d -eq 1 ];then cp -rf $(derived_dir)/../dists/profileDist/%s/* $(derived_dir)/../dists/profileDist/%s/;fi'% (_include_base_profile, mstar_base_profile_file_path, mstar_default_profile_path),
        'if [ %d -eq 1 ];then cp -rf %s/* $(derived_dir)/../dists/profileDist/%s/;fi'% (_include_merge_profile, mstar_base_profile_file_path, mstar_default_profile_path),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/profileDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("profile_config")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/publicDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("utilities_public")),
        'mkdir -p $(derived_dir)/external_delivery/profile_config/mb180/mb180_hoteltv/;',
        'cp -r profile_config/mb180/mb180_hoteltv $(derived_dir)/external_delivery/profile_config/mb180/;'
        
        # copy dependent project includes and libs - not static; should be copied instead of adding into mstar_extra_libs.txt
        'mkdir -p $(derived_dir)/external_delivery/project/libcurl/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/openssl/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/live555/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/libpng/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/zlib/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/freetype/output/;',
        'mkdir -p $(derived_dir)/external_delivery/project/libxml2/output/;',
        'cp -r project/libcurl/output/mb181 $(derived_dir)/external_delivery/project/libcurl/output/;'.format(libcurl_dir),
        'cp -r project/openssl/output/mb181 $(derived_dir)/external_delivery/project/openssl/output/;',
        'cp -r project/live555/output/mb181 $(derived_dir)/external_delivery/project/live555/output/;',
        'cp -r project/libpng/output/mb181 $(derived_dir)/external_delivery/project/libpng/output/;',
        'cp -r project/zlib/output/mb181 $(derived_dir)/external_delivery/project/zlib/output/;',
        'cp -r project/freetype/output/mb181 $(derived_dir)/external_delivery/project/freetype/output/;',
        'cp -r project/libxml2/output/mb181 $(derived_dir)/external_delivery/project/libxml2/output/;',
        

        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/nghttp2/output/;fi' % (_include_options_nghttp2),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/sqlite/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/ffmpeg/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/portaudio/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/webvtt/output/;fi' % (_avs_caption_support),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/avs/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/libwebsockets/output/;fi' % (_include_options_avs),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/asio/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/websocketpp/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/apl/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/mmsdk/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/aws_iot/output/;fi' % (_include_options_aws_iot),
        'if [ %d -eq 1 ]; then mkdir -p $(derived_dir)/external_delivery/project/pubsub/;fi' % (_include_options_extended_logging),
        'if [ %d -eq 1 ]; then cp -r project/nghttp2/output/mb181 $(derived_dir)/external_delivery/project/nghttp2/output/;fi' % (_include_options_nghttp2),
        'if [ %d -eq 1 ]; then cp -r project/sqlite/output/mb181 $(derived_dir)/external_delivery/project/sqlite/output/;fi' % (_include_options_avs),
        'if [ {} -eq 1 ]; then cp -r project/ffmpeg/output/mb181 $(derived_dir)/external_delivery/project/ffmpeg/output/;fi;'.format(_include_options_avs),
        'if [ %d -eq 1 ]; then cp -r project/portaudio/output/mb181 $(derived_dir)/external_delivery/project/portaudio/output/;fi' % (_include_options_avs),
        'if [ {} -eq 1 ]; then cp -r project/webvtt/output/mb181 $(derived_dir)/external_delivery/project/webvtt/output/;fi'.format(_avs_caption_support),
        'if [ {} -eq 1 ]; then cp -r project/avs/output/mb181 $(derived_dir)/external_delivery/project/avs/output/;fi'.format(_include_options_avs),
        'if [ {} -eq 1 ]; then cp -r project/libwebsockets/output/mb181 $(derived_dir)/external_delivery/project/libwebsockets/output/;fi'.format(_include_options_avs),
        'if [ %d -eq 1 ]; then cp -r project/asio/output/mb181 $(derived_dir)/external_delivery/project/asio/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then cp -r project/websocketpp/output/mb181 $(derived_dir)/external_delivery/project/websocketpp/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then cp -r project/apl/output/mb181 $(derived_dir)/external_delivery/project/apl/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then cp -r project/mmsdk/output/mb181 $(derived_dir)/external_delivery/project/mmsdk/output/;fi' % (_include_options_avs_mmsdk),
        'if [ %d -eq 1 ]; then cp -r project/aws_iot/output/mb181 $(derived_dir)/external_delivery/project/aws_iot/output/;fi' % (_include_options_aws_iot),
        'if [ %d -eq 1 ]; then cp -r project/pubsub/* $(derived_dir)/external_delivery/project/pubsub;fi' % (_include_options_extended_logging),

        'echo "#!/bin/bash">$(derived_dir)/external_delivery/copy_folders.sh',
        'echo "cp -rv build_info/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh',
        'echo "cp -rv certificates/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh',
        'if [ %s = "True" ];then echo "cp -rv eclipse_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("eclipse_src")),
        'if [ %s = "True" ];then echo "cp -rv inca/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("inca")),
        'if [ %s = "True" ];then echo "cp -rv libdlna_access_3.0/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("libdlna_access_3.0")),
        'if [ %s = "True" ];then echo "cp -rv lynx/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("lynx")),
        'if [ %s = "True" ];then echo "cp -rv 3rd_party/mplib-1.0.3/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("3rd_party/mplib-1.0.3")),
        'if [ %s = "True" ];then echo "cp -rv svg_components/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("svg_components")),
        'if [ %s = "True" ];then echo "cp -rv profile_config/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("profile_config")),
        'if [ %s = "True" ];then echo "cp -rv utilities_public/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("utilities_public")),
        'echo "cp -rv project/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;',
    )

    if include_options.vewd_browser:
        mstarlib_command += (
         'if [ %s = "True" ];then %s/scripts/create-vewd-package.sh %s %s %s %s %s %s ;fi' % (is_component_used("vewd_common"), project_build_dir, project_build_dir, vewd_sdk_dist_dir, "$(derived_dir)/external_delivery", vewd_hbbtv_dist_dir, vewd_platform, vewd_clib),
         'if [ %s = "True" ];then echo "cp -rv work/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("vewd_common")),
         'mkdir -p $(derived_dir)/external_delivery/%s' % vewd_module,
         'mkdir -p $(derived_dir)/external_delivery/%s/vewd_json' % vewd_module,
         'cp %s/vewd_json/config_MTK.json $(derived_dir)/external_delivery/%s/vewd_json/config_MTK.json' % (vewd_module, vewd_module),
         'if [ %s = "True" ];then echo "cp -rv %s/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi' %(is_component_used(vewd_module), vewd_module),
         'if [ %d -eq 1 ];then mkdir -p $(derived_dir)/external_delivery/project/lighttpd/output/;fi' %(_include_lighttpd_web_server),
         'if [ %d -eq 1 ];then cp -r project/lighttpd/output/mb181 $(derived_dir)/external_delivery/project/lighttpd/output/;fi' %(_include_lighttpd_web_server),
         'if [ %d -eq 1 ];then mkdir -p $(derived_dir)/external_delivery/html_applications/tivo/;fi' %(_include_lighttpd_web_server),
         'if [ %d -eq 1 ];then cp -r %s/html_applications/tivo/%s $(derived_dir)/external_delivery/html_applications/tivo/;fi' %(_include_lighttpd_web_server, project_root, _tivo_app_package),
         'if [ %d -eq 1 ];then cp -r %s/html_applications/tivo/%s $(derived_dir)/external_delivery/html_applications/tivo/;fi' %(_include_prestaging_package, project_root, _tivo_app_package_prestaging),
         'if [ %d -eq 1 ];then echo "cp -rv html_applications/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi' %(_include_lighttpd_web_server),
        )

        if include_options.vewd_sdk423:
            mstarlib_command += create_external_delivery_copy_ipc_command()
            mstarlib_command += create_external_delivery_copy_drmbackend_command()
            mstarlib_command += create_external_delivery_jsplugin_command(jsplugins)

    if include_options.dynamic_ui:
        mstarlib_command += (
        'if [ ! -d $(derived_dir)/external_delivery/dynamic_ui ];then mkdir $(derived_dir)/external_delivery/dynamic_ui;fi',
        'cp -rf %s/environment/release/vendor/dynamic_ui/* $(derived_dir)/external_delivery/dynamic_ui/' % (project_build_dir),
        )

    mstarlib_command += (
        'chmod +x $(derived_dir)/external_delivery/copy_folders.sh',
        # give write permission to files in external_delivery
        'chmod -R +w $(derived_dir)/external_delivery',

        '$(call rmfile,%s)' %(aurora_lib),
        '$(call rmfile,%s)' %(no_access_txt),
        '$(call rmfile,%s)' %(libs_txt),
        )

    amazonlib_command = ('mkdir -p $(derived_dir)/external_delivery/',
        # make aurora_lib for Amazon. Copy library and informative scripts.
        "sed -i '/save/d' %s;cat $(derived_dir)/../scripts/amazon_extra_libs%s.txt >> %s;cat %s;$(call mklib,%s );" % (libs_txt,_bin_file_name_option,libs_txt,libs_txt,libs_txt),
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (libs_txt),
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (aurora_lib),
        '$(call copyfile,%s,$(derived_dir)/external_delivery/)' % (no_access_txt),

        # copy various extra files
        'if [ %s = "True" ];then mkdir -p $(derived_dir)/external_delivery/svg_components;fi'% (is_component_used("svg_components")),
        'if [ %s = "True" ];then cp svg_components/* $(derived_dir)/external_delivery/svg_components/;fi'% (is_component_used("svg_components")),

        'mkdir -p $(derived_dir)/external_delivery/build_info',
        'cp $(derived_dir)/build_info/* $(derived_dir)/external_delivery/build_info/',

        'mkdir -p $(derived_dir)/external_delivery/certificates',
        'cp certificates/cacert.pem $(derived_dir)/external_delivery/certificates/cacert.pem',
        'cp certificates/prod-partner-netflix-net-chain.pem $(derived_dir)/external_delivery/certificates/prod-partner-netflix-net-chain.pem',

        # copy files indicated by forbidden dist.
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/DvbDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("dvb")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/EclipseSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("eclipse_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/EclipseDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("eclipse")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/FVCDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("fvc")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/NebulaSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("nebula_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/NovoDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("novo")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/IncaDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("inca")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/LibdlnaDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("libdlna_access_3.0")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/LynxDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("lynx")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/MplibDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("3rd_party/mplib-1.0.3")),
        'if [ %d -eq 1 ];then cp -rf $(derived_dir)/../dists/profileDist/%s/* $(derived_dir)/../dists/profileDist/%s/;fi'% (_include_base_profile, base_profile_file_path, default_profile_path),
        'if [ %d -eq 1 ];then cp -rf %s/* $(derived_dir)/../dists/profileDist/%s/;fi'% (_include_merge_profile, base_profile_file_path, default_profile_path),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/profileDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("profile_config")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/Sqlite3WrapperDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("3rd_party_wrapper/sqlite3")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/TinyxmlWrapperDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("3rd_party_wrapper/tinyxml")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/UtilitiesPrivateSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("utilities_private_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/UtilitiesPublicSrcDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("utilities_public_src")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/LanguageDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("language")),
        'if [ %s = "True" ];then cp -r $(derived_dir)/../dists/HttpClientDist/* $(derived_dir)/external_delivery/;fi'% (is_component_used("http_client")),

        # We will deliver above files to external users. The script generated below copies these files to their own working copy.
        'echo "#!/bin/bash">$(derived_dir)/external_delivery/copy_folders.sh',
        'echo "cp -rv build_info/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh',
        'echo "cp -rv certificates/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh',
        'if [ %s = "True" ];then echo "cp -rv dvb/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("dvb")),
        'if [ %s = "True" ];then echo "cp -rv eclipse/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("eclipse")),
        'if [ %s = "True" ];then echo "cp -rv eclipse_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("eclipse_src")),
        'if [ %s = "True" ];then echo "cp -rv fvc/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("fvc")),
        'if [ %s = "True" ];then echo "cp -rv inca/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("inca")),
        'if [ %s = "True" ];then echo "cp -rv libdlna_access_3.0/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("libdlna_access_3.0")),
        'if [ %s = "True" ];then echo "cp -rv lynx/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("lynx")),
        'if [ %s = "True" ];then echo "cp -rv mplib-1.0.3/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("mplib-1.0.3")),
        'if [ %s = "True" ];then echo "cp -rv nebula_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("nebula_src")),
        'if [ %s = "True" ];then echo "cp -rv novo/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("novo")),
        'if [ %s = "True" ];then echo "cp -rv svg_components/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("svg_components")),
        'if [ %s = "True" ];then echo "cp -rv 3rd_party_wrapper/ ../../../3rd_party_wrapper/">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%
            (is_component_used("3rd_party_wrapper/sqlite3") or
             is_component_used("3rd_party_wrapper/tinyxml")),
        'if [ %s = "True" ];then echo "cp -rv profile_config/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("profile_config")),
        'if [ %s = "True" ];then echo "cp -rv utilities_private_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("utilities_private_src")),
        'if [ %s = "True" ];then echo "cp -rv utilities_public_src/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("utilities_public_src")),
        'if [ %s = "True" ];then echo "cp -rv language/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("language")),
        'if [ %s = "True" ];then echo "cp -rv http_client/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'% (is_component_used("http_client")),
        #'echo "cp -rv project/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;',
    )

    if include_options.vewd_browser:
        amazonlib_command += (
         'if [ %s = "True" ];then %s/scripts/create-vewd-package.sh %s %s %s %s %s %s ;fi' % (is_component_used("vewd_common"), project_build_dir, project_build_dir, vewd_sdk_dist_dir, "$(derived_dir)/external_delivery", vewd_hbbtv_dist_dir, vewd_platform, vewd_clib),
         #'if [ %s = "True" ];then echo "cp -rv work/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi'%(is_component_used("vewd_common")),
         'mkdir -p $(derived_dir)/external_delivery/%s' % vewd_module,
         'mkdir -p $(derived_dir)/external_delivery/%s/vewd_json' % vewd_module,
         'cp %s/vewd_json/config_MTK.json $(derived_dir)/external_delivery/%s/vewd_json/config_MTK.json' % (vewd_module, vewd_module),
         'if [ %s = "True" ];then echo "cp -rv %s/ ../../../">>$(derived_dir)/external_delivery/copy_folders.sh;fi' %(is_component_used(vewd_module), vewd_module),
        )

        if include_options.vewd_sdk423:
            amazonlib_command += create_external_delivery_copy_ipc_command()
            amazonlib_command += create_external_delivery_copy_drmbackend_command()
            amazonlib_command += create_external_delivery_jsplugin_command(jsplugins)

    if include_options.dynamic_ui:
        amazonlib_command += (
        'if [ ! -d $(derived_dir)/external_delivery/dynamic_ui ];then mkdir $(derived_dir)/external_delivery/dynamic_ui;fi',
        'cp -rf %s/environment/release/vendor/dynamic_ui/* $(derived_dir)/external_delivery/dynamic_ui/' % (project_build_dir),
        ) 
    amazonlib_command +=(
        # make copy script executable.
        'chmod +x $(derived_dir)/external_delivery/copy_folders.sh',
        # give write permission to files in external_delivery
        'chmod -R +w $(derived_dir)/external_delivery',

        # remove unnecessary files
        '$(call rmfile,%s)' %(aurora_lib),
        '$(call rmfile,%s)' %(no_access_txt),
        '$(call rmfile,%s)' %(libs_txt),
        )

    special_build(
        'vewdlib',
        command = vewdlib_command,
        phony = True,
        requires = 'elf copy_default_ui',
        )

    special_build(
        'mstarlib',
        command = mstarlib_command,
        phony = True,
        requires = 'elf copy_default_ui',
        )

    special_build(
        'amazonlib',
        command = amazonlib_command,
        phony = True,
        requires = 'elf copy_default_ui',
        )

if 'build_dynamic_ui' not in options.targets:
    if 'createDist' in options.targets:
        if include_options.ci:
            use_interface('aurora_ci_binder')
            use_interface('inca')
        use_interface('crypto_src')
        if include_options.forcedsmcc:
            use_interface('dsmcc_src')
        if include_options.mheg:
            use_interface('mheg')
        if include_options.robotester:
            use_interface('robotester')
        use_interface('dvb')
        use_interface('eclipse')
        use_interface('eclipse_src',
            TIME_STORE_TO_NVDS = _time_store_to_nvds,
            )
        if (include_options.dlna
                and include_options.nflc30
                and include_options.nflc):
            use_interface('libdlna_access_3.0')
        use_interface('lynx')
        use_interface('3rd_party/mplib-1.0.3')
        use_interface('profile_config')
        use_interface('utilities_public')
        if (include_options.hbbtv 
                or include_options.portal
                or include_options.iplayer
                or include_options.ip_stb):
            use_interface('nebula_src')
            if include_options.vewd_sdk423:
                use_interface('3rd_party_wrapper/rpclib')
        use_interface('novo')
        if include_options.fvc:
            use_interface('fvc')
        if include_options.sqlite:
            use_interface('3rd_party_wrapper/sqlite3')
        use_interface('3rd_party_wrapper/tinyxml')
        use_interface('utilities_private_src')
        use_interface('utilities_public_src')
        use_interface('language')
        use_interface('http_client')
        use_interface('3rd_party_wrapper/curl')

    dist_command = ('rm -rf $(derived_dir)/../dists',
        'mkdir -p $(derived_dir)/../dists',
        'mv $(derived_dir)/../AuroraCiBinderDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../CRYPTODist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../DSMCCDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../MHEGDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../RobotesterDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../DvbDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../EclipseDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../EclipseSrcDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../FVCDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../IncaDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../LibdlnaDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../LynxDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../MplibDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../profileDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../publicDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../NebulaSrcDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../NovoDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../Sqlite3WrapperDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../TinyxmlWrapperDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../UtilitiesPrivateSrcDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../UtilitiesPublicSrcDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../LanguageDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../HttpClientDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../CurlWrapperDist/ $(derived_dir)/../dists/',
        'mv $(derived_dir)/../RpclibWrapperDist/ $(derived_dir)/../dists/',
        )

    extra_headers = ['AuroraCiBinderDist','CRYPTODist','DSMCCDist','MHEGDist','RobotesterDist',
        'EclipseDist', 'EclipseSrcDist', 'DvbDist', 'IncaDist', 'LibdlnaDist',
        'LynxDist', 'MplibDist', 'profileDist', 'publicDist', 'NebulaSrcDist',
        'NovoDist', 'FVCDist', 'Sqlite3WrapperDist', 'TinyxmlWrapperDist',
        'UtilitiesPrivateSrcDist', 'UtilitiesPublicSrcDist', 'LanguageDist', 'HttpClientDist',
        'CurlWrapperDist', 'RpclibWrapperDist']

    special_build(
        'createDist',
        command = dist_command,
        phony = True,
        requires = extra_headers,
        )

    make_forbidden_dist(
        'AuroraCiBinderDist',
        'aurora_ci_binder',
        )
    make_forbidden_dist(
        'CRYPTODist',
        'crypto_src',
        )
    make_forbidden_dist(
        'DSMCCDist',
        'dsmcc_src',
        )
    make_forbidden_dist(
        'MHEGDist',
        'mheg',
        )
    make_forbidden_dist(
        'RobotesterDist',
        'robotester',
        )
    make_forbidden_dist(
        'DvbDist',
        'dvb',
        )
    make_forbidden_dist(
        'EclipseDist',
        'eclipse',
        )
    make_forbidden_dist(
        'EclipseSrcDist',
        'eclipse_src',
        )
    make_forbidden_dist(
        'IncaDist',
        'inca',
        )
    make_forbidden_dist(
        'LibdlnaDist',
        'libdlna_access_3.0',
        )
    make_forbidden_dist(
        'LynxDist',
        'lynx',
        )
    make_forbidden_dist(
        'MplibDist',
        '3rd_party/mplib-1.0.3',
        )
    make_forbidden_dist(
        'profileDist',
        'profile_config',
        )
    make_forbidden_dist(
        'publicDist',
        'utilities_public',
        )
    make_forbidden_dist(
        'NebulaSrcDist',
        'nebula_src',
        )
    make_forbidden_dist(
        'NovoDist',
        'novo',
        )
    make_forbidden_dist(
        'FVCDist',
        'fvc',
        )
    make_forbidden_dist(
        'Sqlite3WrapperDist',
        '3rd_party_wrapper/sqlite3',
        )
    make_forbidden_dist(
        'TinyxmlWrapperDist',
        '3rd_party_wrapper/tinyxml',
        )
    make_forbidden_dist(
        'UtilitiesPrivateSrcDist',
        'utilities_private_src',
        )
    make_forbidden_dist(
        'UtilitiesPublicSrcDist',
        'utilities_public_src',
        )
    make_forbidden_dist(
        'LanguageDist',
        'language',
        )
    make_forbidden_dist(
        'HttpClientDist',
        'http_client',
        )
    make_forbidden_dist(
        'CurlWrapperDist',
        '3rd_party_wrapper/curl',
        )
    make_forbidden_dist(
        'RpclibWrapperDist',
        '3rd_party_wrapper/rpclib',
        )

conf_env = cabot_env.environ_linux()
