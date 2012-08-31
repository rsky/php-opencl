/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "device.h"

/* {{{ type definitions */

typedef enum {
	PARAM_TYPE_BITFIELD = 0,
	PARAM_TYPE_BOOL,
	PARAM_TYPE_SIZE,
	PARAM_TYPE_UINT,
	PARAM_TYPE_ULONG,
	PARAM_TYPE_STRING,
	PARAM_TYPE_PLATFORM,
	PARAM_TYPE_MAX_WORK_ITEM_SIZES,
} phpcl_param_type_t;

typedef struct {
	const char *key;
	cl_device_info name;
	phpcl_param_type_t type;
} phpcl_device_info_param_t;

/* }}} */
/* {{{ globals */

static const phpcl_device_info_param_t device_info_list[] = {
	{ "type",                          CL_DEVICE_TYPE,                          PARAM_TYPE_BITFIELD  },
	{ "vendor_id",                     CL_DEVICE_VENDOR_ID,                     PARAM_TYPE_UINT      },
	{ "max_compute_units",             CL_DEVICE_MAX_COMPUTE_UNITS,             PARAM_TYPE_UINT      },
	{ "max_work_item_dimensions",      CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,      PARAM_TYPE_UINT      },
	{ "max_work_group_size",           CL_DEVICE_MAX_WORK_GROUP_SIZE,           PARAM_TYPE_SIZE      },
	{ "max_work_item_sizes",           CL_DEVICE_MAX_WORK_ITEM_SIZES, PARAM_TYPE_MAX_WORK_ITEM_SIZES },
	{ "preferred_vector_width_char",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,   PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_short",  CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,  PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_int",    CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,    PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_long",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,   PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_float",  CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,  PARAM_TYPE_SIZE      },
	{ "preferred_vector_width_double", CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, PARAM_TYPE_SIZE      },
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF /* OpenCL 1.1 */
	{ "preferred_vector_width_half",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF,   PARAM_TYPE_UINT      },
#endif
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR /* OpenCL 1.1 */
	{ "native_vector_width_char",      CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR,      PARAM_TYPE_UINT      },
	{ "native_vector_width_short",     CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT,     PARAM_TYPE_UINT      },
	{ "native_vector_width_int",       CL_DEVICE_NATIVE_VECTOR_WIDTH_INT,       PARAM_TYPE_UINT      },
	{ "native_vector_width_long",      CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG,      PARAM_TYPE_UINT      },
	{ "native_vector_width_float",     CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT,     PARAM_TYPE_UINT      },
	{ "native_vector_width_double",    CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE,    PARAM_TYPE_UINT      },
	{ "native_vector_width_half",      CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF,      PARAM_TYPE_UINT      },
#endif
	{ "max_clock_frequency",           CL_DEVICE_MAX_CLOCK_FREQUENCY,           PARAM_TYPE_UINT      },
	{ "address_bits",                  CL_DEVICE_ADDRESS_BITS,                  PARAM_TYPE_UINT      },
	{ "max_read_image_args",           CL_DEVICE_MAX_READ_IMAGE_ARGS,           PARAM_TYPE_UINT      },
	{ "max_write_image_args",          CL_DEVICE_MAX_WRITE_IMAGE_ARGS,          PARAM_TYPE_UINT      },
	{ "max_mem_alloc_size",            CL_DEVICE_MAX_MEM_ALLOC_SIZE,            PARAM_TYPE_ULONG     },
	{ "image2d_max_width",             CL_DEVICE_IMAGE2D_MAX_WIDTH,             PARAM_TYPE_SIZE      },
	{ "image2d_max_height",            CL_DEVICE_IMAGE2D_MAX_HEIGHT,            PARAM_TYPE_SIZE      },
	{ "image3d_max_width",             CL_DEVICE_IMAGE3D_MAX_WIDTH,             PARAM_TYPE_SIZE      },
	{ "image3d_max_height",            CL_DEVICE_IMAGE3D_MAX_HEIGHT,            PARAM_TYPE_SIZE      },
	{ "image3d_max_depth",             CL_DEVICE_IMAGE3D_MAX_DEPTH,             PARAM_TYPE_SIZE      },
	{ "image_support",                 CL_DEVICE_IMAGE_SUPPORT,                 PARAM_TYPE_BOOL      },
	{ "max_parameter_size",            CL_DEVICE_MAX_PARAMETER_SIZE,            PARAM_TYPE_SIZE      },
	{ "max_samplers",                  CL_DEVICE_MAX_SAMPLERS,                  PARAM_TYPE_UINT      },
	{ "mem_base_addr_align",           CL_DEVICE_MEM_BASE_ADDR_ALIGN,           PARAM_TYPE_UINT      },
	{ "min_data_type_align_size",      CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,      PARAM_TYPE_UINT      },
	{ "single_fp_config",              CL_DEVICE_SINGLE_FP_CONFIG,              PARAM_TYPE_BITFIELD  },
	{ "global_mem_cache_type",         CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,         PARAM_TYPE_UINT      },
	{ "global_mem_cacheline_size",     CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,     PARAM_TYPE_UINT      },
	{ "global_mem_cache_size",         CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,         PARAM_TYPE_ULONG     },
	{ "global_mem_size",               CL_DEVICE_GLOBAL_MEM_SIZE,               PARAM_TYPE_ULONG     },
	{ "max_constant_buffer_size",      CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,      PARAM_TYPE_ULONG     },
	{ "max_constant_args",             CL_DEVICE_MAX_CONSTANT_ARGS,             PARAM_TYPE_UINT      },
	{ "local_mem_type",                CL_DEVICE_LOCAL_MEM_TYPE,                PARAM_TYPE_UINT      },
	{ "local_mem_size",                CL_DEVICE_LOCAL_MEM_SIZE,                PARAM_TYPE_ULONG     },
	{ "error_correction_support",      CL_DEVICE_ERROR_CORRECTION_SUPPORT,      PARAM_TYPE_BOOL      },
	{ "profiling_timer_resolution",    CL_DEVICE_PROFILING_TIMER_RESOLUTION,    PARAM_TYPE_SIZE      },
	{ "endian_little",                 CL_DEVICE_ENDIAN_LITTLE,                 PARAM_TYPE_BOOL      },
	{ "available",                     CL_DEVICE_AVAILABLE,                     PARAM_TYPE_BOOL      },
	{ "compiler_available",            CL_DEVICE_COMPILER_AVAILABLE,            PARAM_TYPE_BOOL      },
	{ "execution_capabilities",        CL_DEVICE_EXECUTION_CAPABILITIES,        PARAM_TYPE_BITFIELD  },
	{ "queue_properties",              CL_DEVICE_QUEUE_PROPERTIES,              PARAM_TYPE_BITFIELD  },
	{ "name",                          CL_DEVICE_NAME,                          PARAM_TYPE_STRING    },
	{ "vendor",                        CL_DEVICE_VENDOR,                        PARAM_TYPE_STRING    },
	{ "driver_version",                CL_DRIVER_VERSION,                       PARAM_TYPE_STRING    },
	{ "profile",                       CL_DEVICE_PROFILE,                       PARAM_TYPE_STRING    },
	{ "version",                       CL_DEVICE_VERSION,                       PARAM_TYPE_STRING    },
	{ "extensions",                    CL_DEVICE_EXTENSIONS,                    PARAM_TYPE_STRING    },
	{ "platform",                      CL_DEVICE_PLATFORM,                      PARAM_TYPE_PLATFORM  },
#ifdef CL_DEVICE_DOUBLE_FP_CONFIG
	{ "double_fp_config",              CL_DEVICE_DOUBLE_FP_CONFIG,              PARAM_TYPE_BITFIELD  },
#endif
#ifdef CL_DEVICE_HALF_FP_CONFIG
	{ "half_fp_config",                CL_DEVICE_HALF_FP_CONFIG,                PARAM_TYPE_BITFIELD  },
#endif
#ifdef CL_DEVICE_HOST_UNIFIED_MEMORY
	{ "host_unified_memory",           CL_DEVICE_HOST_UNIFIED_MEMORY,           PARAM_TYPE_BOOL      },
#endif
#ifdef CL_DEVICE_OPENCL_C_VERSION
	{ "opencl_c_version",              CL_DEVICE_OPENCL_C_VERSION,              PARAM_TYPE_STRING    },
#endif
	{ NULL, 0, 0 }
};

/* }}} */
/* {{{ phpcl_get_device_info() */

static zval *phpcl_get_device_info(cl_device_id device TSRMLS_DC)
{
	const phpcl_device_info_param_t *param = device_info_list;
	cl_int err = CL_SUCCESS;
	char buf[1024] = { 0 };
	size_t len = 0;
	cl_uint max_work_item_dimensions = 0;
	zval *zinfo;

	err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
	                      sizeof(max_work_item_dimensions),
	                      &max_work_item_dimensions, NULL);
	if (err != CL_SUCCESS) {
		max_work_item_dimensions = 0;
	}

	MAKE_STD_ZVAL(zinfo);
	array_init_size(zinfo, 64);

	snprintf(buf, sizeof(buf), "%p", device);
	add_assoc_string(zinfo, "id", buf, 1);

	while (param->key != NULL) {
		switch (param->type) {
			case PARAM_TYPE_BITFIELD: {
				cl_bitfield val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					long lval = (long)val;
					add_assoc_long(zinfo, param->key, lval);
				} 
			}
			break;

			case PARAM_TYPE_BOOL: {
				cl_bool val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					zend_bool bval = (zend_bool)val;
					add_assoc_bool(zinfo, param->key, bval);
				}
			}
			break;

			case PARAM_TYPE_SIZE: {
				size_t val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					long lval = (long)val;
					add_assoc_long(zinfo, param->key, lval);
				}
			}
			break;

			case PARAM_TYPE_UINT: {
				cl_uint val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					long lval = (long)val;
					add_assoc_long(zinfo, param->key, lval);
				}
			}
			break;

			case PARAM_TYPE_ULONG: {
				cl_ulong val = 0;
				err = clGetDeviceInfo(device, param->name, sizeof(val), &val, NULL);
				if (err == CL_SUCCESS) {
					long lval = (long)val;
					add_assoc_long(zinfo, param->key, lval);
				}
			}
			break;

			case PARAM_TYPE_STRING: {
				err = clGetDeviceInfo(device, param->name, sizeof(buf), buf, &len);
				if (err == CL_SUCCESS) {
					add_assoc_stringl(zinfo, param->key, buf, len, 1);
				}
			}
			break;

			case PARAM_TYPE_PLATFORM: {
				cl_platform_id platform;
				err = clGetDeviceInfo(device, param->name, sizeof(cl_platform_id), &platform, NULL);
				if (err == CL_SUCCESS) {
					zval *entry;
					MAKE_STD_ZVAL(entry);
					ZEND_REGISTER_RESOURCE(entry, platform, phpcl_le_platform());
					add_assoc_zval(zinfo, param->key, entry);
				}
			}
			break;

			case PARAM_TYPE_MAX_WORK_ITEM_SIZES: {
				size_t siz = sizeof(size_t) * max_work_item_dimensions;
				size_t *sizes = ecalloc(max_work_item_dimensions, sizeof(size_t));
				if (!sizes) {
					break;
				}

				err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, siz, sizes, NULL);
				if (err == CL_SUCCESS) {
					cl_uint i;
					zval *zsizes;
					MAKE_STD_ZVAL(zsizes);
					array_init_size(zsizes, max_work_item_dimensions);
					for (i = 0; i < max_work_item_dimensions; i++) {
						long lval = (long)sizes[i];
						add_next_index_long(zsizes, lval);
					}
					add_assoc_zval(zinfo, param->key, zsizes);
				}

				efree(sizes);
			}
			break;

			default:
				err = CL_SUCCESS;
				add_assoc_null(zinfo, param->key);
		}

		if (err != CL_SUCCESS) {
			add_assoc_null(zinfo, param->key);
		}

		param++;
	}

	return zinfo;
}

/* }}} */
/* {{{ array cl_get_device_ids([resource cl_platform_id platform[, int device_type]]) */

PHP_FUNCTION(cl_get_device_ids)
{
	zval *zid = NULL;
	cl_platform_id platform = NULL;
	long ltype = 0L;
	cl_device_type device_type = CL_DEVICE_TYPE_DEFAULT;

	cl_int err = CL_SUCCESS;
	cl_uint num_entries = 0;
	cl_device_id *devices = NULL;
	cl_uint index = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "|r!l", &zid, &ltype) == FAILURE) {
		return;
	}
	if (zid) {
		ZEND_FETCH_RESOURCE(platform, cl_platform_id, &zid, -1,
		                    "cl_platform_id", phpcl_le_platform());
	}
	if (ZEND_NUM_ARGS() == 2) {
		device_type = (cl_device_type)ltype;
	}

	err = clGetDeviceIDs(platform, device_type, 0, NULL, &num_entries);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetDeviceIDs() failed [%s]", phpcl_errstr(err));
		efree(devices);
		return;
	}

	devices = ecalloc(num_entries, sizeof(cl_device_id));
	if (!devices) {
		return;
	}

	err = clGetDeviceIDs(platform, device_type, num_entries, devices, NULL);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetDeviceIDs() failed [%s]", phpcl_errstr(err));
		efree(devices);
		return;
	}

	array_init_size(return_value, num_entries);

	for (index = 0; index < num_entries; index++) {
		cl_device_id device = devices[index];
		zval *entry;
		MAKE_STD_ZVAL(entry);
		ZEND_REGISTER_RESOURCE(entry, device, phpcl_le_device());
		add_next_index_zval(return_value, entry);
	}

	efree(devices);
}

/* }}} */
/* {{{ array cl_get_device_info(resource cl_device_id device) */

PHP_FUNCTION(cl_get_device_info)
{
	zval *zid = NULL;
	cl_device_id device = NULL;
	zval *device_info = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r", &zid) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(device, cl_device_id, &zid, -1,
	                    "cl_device_id", phpcl_le_device());

	device_info = phpcl_get_device_info(device TSRMLS_CC);
	if (device_info) {
		RETURN_ZVAL(device_info, 0, 1);
	}
}

/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
