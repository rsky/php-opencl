/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "php_opencl.h"
#include "params.h"
#include "platform.h"
#include "device.h"
#include "context.h"
#include "queue.h"
#include "memory.h"
#include "program.h"
#include "kernel.h"
#include "event.h"
#include "sampler.h"

/* {{{ globals */

static int le_platform;
static int le_device;
static int le_context;
static int le_command_queue;
static int le_mem;
static int le_event;
static int le_program;
static int le_kernel;
static int le_sampler;

/* }}} */
/* {{{ function prototypes */

/* module functions */
static PHP_MINIT_FUNCTION(opencl);
static PHP_MINFO_FUNCTION(opencl);

/* module function helpers */
static void _register_constants(int module_number TSRMLS_DC);
static void _register_resources(int module_number TSRMLS_DC);

/* resource destructors */
static void _destroy_context(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _destroy_command_queue(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _destroy_mem(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _destroy_program(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _destroy_kernel(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _destroy_event(zend_rsrc_list_entry *rsrc TSRMLS_DC);
static void _destroy_sampler(zend_rsrc_list_entry *rsrc TSRMLS_DC);

/* }}} */
/* {{{ argument informations */

/* platform */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_platform_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, platform)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* device */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_device_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, device)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_device_ids, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, platform)
	ZEND_ARG_INFO(0, device_type)
ZEND_END_ARG_INFO()

/* context */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_context_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_context, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, devices)
	ZEND_ARG_ARRAY_INFO(0, properties, 1)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, userdata)
	ZEND_ARG_INFO(1, errcode)
ZEND_END_ARG_INFO()

/* command queue */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_command_queue_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, command_queue)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_command_queue, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, device)
	ZEND_ARG_INFO(0, properties)
	ZEND_ARG_INFO(1, errcode)
ZEND_END_ARG_INFO()

/* mem */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_mem_object_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, memobj)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/*ZEND_BEGIN_ARG_INFO_EX(arginfo_get_image_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, image)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()*/

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_buffer, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 3)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

/* program */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_program_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, program)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_program_with_source, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, errcode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_build_program, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, program)
	ZEND_ARG_INFO(0, devices)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, userdata)
ZEND_END_ARG_INFO()

/* kernel */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_kernel_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, kernel)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_create_kernel, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, program)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* event */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_event_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* sampler */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_sampler_info, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, sampler)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* }}} */
/* {{{ phpcl_functions */

static zend_function_entry phpcl_functions[] = {
	/* platform */
	PHP_FE(cl_get_platform_info,            arginfo_get_platform_info)
	PHP_FE(cl_get_platform_ids,             NULL)
	/* device */
	PHP_FE(cl_get_device_info,              arginfo_get_device_info)
	PHP_FE(cl_get_device_ids,               arginfo_get_device_ids)
	/* context */
	PHP_FE(cl_get_context_info,             arginfo_get_context_info)
	PHP_FE(cl_create_context,               arginfo_create_context)
	/* command queue */
	PHP_FE(cl_get_command_queue_info,       arginfo_get_command_queue_info)
	PHP_FE(cl_create_command_queue,         arginfo_create_command_queue)
	/* mem */
	PHP_FE(cl_get_mem_object_info,          arginfo_get_mem_object_info)
	/*PHP_FE(cl_get_image_info,               arginfo_get_image_info)*/
	PHP_FE(cl_create_buffer,                arginfo_create_buffer)
	/* program */
	PHP_FE(cl_get_program_info,             arginfo_get_program_info)
	PHP_FE(cl_create_program_with_source,   arginfo_create_program_with_source)
	PHP_FE(cl_build_program,                arginfo_build_program)
	/* kernel */
	PHP_FE(cl_get_kernel_info,              arginfo_get_kernel_info)
	PHP_FE(cl_create_kernel,                arginfo_create_kernel)
	/* event */
	PHP_FE(cl_get_event_info,               arginfo_get_event_info)
	/* sampler */
	PHP_FE(cl_get_sampler_info,             arginfo_get_sampler_info)
	/* terminate */
	{ NULL, NULL, NULL }
};

/* }}} */
/* {{{ cross-extension dependencies */

static zend_module_dep phpcl_deps[] = {
	/*ZEND_MOD_OPTIONAL("gd")*/
	/*ZEND_MOD_OPTIONAL("imagick")*/
	{NULL, NULL, NULL, 0}
};

/* }}} */
/* {{{ opencl_module_entry */

zend_module_entry opencl_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	phpcl_deps,
	"opencl",
	phpcl_functions,
	PHP_MINIT(opencl),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(opencl),
	PHPCL_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_OPENCL
ZEND_GET_MODULE(opencl)
#endif

/* }}} */
/* {{{ PHP_MINIT_FUNCTION */

static PHP_MINIT_FUNCTION(opencl)
{
	_register_constants(module_number TSRMLS_CC);
	_register_resources(module_number TSRMLS_CC);
	return SUCCESS;
}

/* }}} */
/* {{{ PHP_MINFO_FUNCTION */

static PHP_MINFO_FUNCTION(opencl)
{
}

/* }}} */
/* {{{ resource destructors */

static void _destroy_context(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_context_t *ctx = (phpcl_context_t *)rsrc->ptr;
	clReleaseContext(ctx->context);
	if (ctx->callback) {
		zval_ptr_dtor(&ctx->callback);
	}
	if (ctx->data) {
		zval_ptr_dtor(&ctx->data);
	}
	efree(ctx);
	rsrc->ptr = NULL;
}

static void _destroy_command_queue(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	clReleaseCommandQueue((cl_command_queue)rsrc->ptr);
	rsrc->ptr = NULL;
}

static void _destroy_mem(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_memobj_t *mem = (phpcl_memobj_t *)rsrc->ptr;
	clReleaseMemObject(mem->memobj);
	if (mem->ptr) {
		efree(mem->ptr);
	}
	rsrc->ptr = NULL;
}

static void _destroy_program(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	phpcl_program_t *prg = (phpcl_program_t *)rsrc->ptr;
	clReleaseProgram(prg->program);
	if (prg->callback) {
		zval_ptr_dtor(&prg->callback);
	}
	if (prg->data) {
		zval_ptr_dtor(&prg->data);
	}
	efree(prg);
	rsrc->ptr = NULL;
}

static void _destroy_kernel(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	clReleaseKernel((cl_kernel)rsrc->ptr TSRMLS_CC);
	rsrc->ptr = NULL;
}

static void _destroy_event(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	clReleaseEvent((cl_event)rsrc->ptr);
	rsrc->ptr = NULL;
}

static void _destroy_sampler(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	clReleaseSampler((cl_sampler)rsrc->ptr);
	rsrc->ptr = NULL;
}

/* }}} */
/* {{{ phpcl_get_info() */

zval *phpcl_get_info(phpcl_get_info_func_t get_info,
                     phpcl_get_info_ex_func_t get_info_ex,
                     void *obj1, void *obj2,
                     const phpcl_info_param_t *param TSRMLS_DC)
{
	cl_int errcode = CL_SUCCESS;
	zval *zinfo = NULL;

	if (param->type != INFO_TYPE_EXTRA) {
		MAKE_STD_ZVAL(zinfo);
	}

	switch (param->type) {
		case INFO_TYPE_BITFIELD: {
			cl_bitfield val = 0;
			errcode = get_info(obj1, obj2, param->name, sizeof(cl_bitfield), &val, NULL);
			if (errcode == CL_SUCCESS) {
				ZVAL_LONG(zinfo, (long)val);
			} 
		}
		break;

		case INFO_TYPE_BOOL: {
			cl_bool val = 0;
			errcode = get_info(obj1, obj2, param->name, sizeof(cl_bool), &val, NULL);
			if (errcode == CL_SUCCESS) {
				ZVAL_LONG(zinfo, (long)val);
			}
		}
		break;

		case INFO_TYPE_SIZE: {
			size_t val = 0;
			errcode = get_info(obj1, obj2, param->name, sizeof(size_t), &val, NULL);
			if (errcode == CL_SUCCESS) {
				ZVAL_LONG(zinfo, (long)val);
			}
		}
		break;

		case INFO_TYPE_UINT: {
			cl_uint val = 0;
			errcode = get_info(obj1, obj2, param->name, sizeof(cl_uint), &val, NULL);
			if (errcode == CL_SUCCESS) {
				ZVAL_LONG(zinfo, (long)val);
			}
		}
		break;

		case INFO_TYPE_ULONG: {
			cl_ulong val = 0;
			errcode = get_info(obj1, obj2, param->name, sizeof(cl_ulong), &val, NULL);
			if (errcode == CL_SUCCESS) {
				ZVAL_LONG(zinfo, (long)val);
			}
		}
		break;

		case INFO_TYPE_STRING: {
			char buf[1024] = { 0 };
			size_t len = 0;
			errcode = get_info(obj1, obj2, param->name, sizeof(buf), buf, &len);
			if (errcode == CL_SUCCESS) {
				ZVAL_STRINGL(zinfo, buf, len, 1);
			}
		}
		break;

		case INFO_TYPE_PLATFORM: {
			cl_platform_id platform;
			errcode = get_info(obj1, obj2, param->name, sizeof(cl_platform_id), &platform, NULL);
			if (errcode == CL_SUCCESS) {
				ZEND_REGISTER_RESOURCE(zinfo, platform, le_platform);
			}
		}
		break;

		case INFO_TYPE_EXTRA: {
			zval *entry = get_info_ex(obj1, obj2, param->name TSRMLS_CC);
			if (entry) {
				zinfo = entry;
			}
		}
		break;

		default:
			ZVAL_NULL(zinfo);
	}

	if (errcode != CL_SUCCESS) {
		ZVAL_NULL(zinfo);
	}

	return zinfo;
}

// }}}
/* {{{ _register_resources() */

#define _REGISTER_RESOURCE(name) \
	le_##name = zend_register_list_destructors_ex(\
		_destroy_##name, NULL, "cl_" #name, module_number)

static void _register_resources(int module_number TSRMLS_DC)
{
	le_platform = zend_register_list_destructors_ex(
		NULL, NULL, "cl_platform", module_number);
	le_device = zend_register_list_destructors_ex(
		NULL, NULL, "cl_device", module_number);
	_REGISTER_RESOURCE(context);
	_REGISTER_RESOURCE(command_queue);
	_REGISTER_RESOURCE(mem);
	_REGISTER_RESOURCE(program);
	_REGISTER_RESOURCE(kernel);
	_REGISTER_RESOURCE(event);
	_REGISTER_RESOURCE(sampler);
}

/* }}} */
/* {{{ _register_constants() */

#define _REGISTER_CONSTANT(name) \
	REGISTER_LONG_CONSTANT(#name, name, CONST_PERSISTENT | CONST_CS)

static void _register_constants(int module_number TSRMLS_DC)
{
	/* Error Codes */
	_REGISTER_CONSTANT(CL_SUCCESS);
	_REGISTER_CONSTANT(CL_DEVICE_NOT_FOUND);
	_REGISTER_CONSTANT(CL_DEVICE_NOT_AVAILABLE);
	_REGISTER_CONSTANT(CL_COMPILER_NOT_AVAILABLE);
	_REGISTER_CONSTANT(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	_REGISTER_CONSTANT(CL_OUT_OF_RESOURCES);
	_REGISTER_CONSTANT(CL_OUT_OF_HOST_MEMORY);
	_REGISTER_CONSTANT(CL_PROFILING_INFO_NOT_AVAILABLE);
	_REGISTER_CONSTANT(CL_MEM_COPY_OVERLAP);
	_REGISTER_CONSTANT(CL_IMAGE_FORMAT_MISMATCH);
	_REGISTER_CONSTANT(CL_IMAGE_FORMAT_NOT_SUPPORTED);
	_REGISTER_CONSTANT(CL_BUILD_PROGRAM_FAILURE);
	_REGISTER_CONSTANT(CL_MAP_FAILURE);
	_REGISTER_CONSTANT(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	_REGISTER_CONSTANT(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);

	_REGISTER_CONSTANT(CL_INVALID_VALUE);
	_REGISTER_CONSTANT(CL_INVALID_DEVICE_TYPE);
	_REGISTER_CONSTANT(CL_INVALID_PLATFORM);
	_REGISTER_CONSTANT(CL_INVALID_DEVICE);
	_REGISTER_CONSTANT(CL_INVALID_CONTEXT);
	_REGISTER_CONSTANT(CL_INVALID_QUEUE_PROPERTIES);
	_REGISTER_CONSTANT(CL_INVALID_COMMAND_QUEUE);
	_REGISTER_CONSTANT(CL_INVALID_HOST_PTR);
	_REGISTER_CONSTANT(CL_INVALID_MEM_OBJECT);
	_REGISTER_CONSTANT(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
	_REGISTER_CONSTANT(CL_INVALID_IMAGE_SIZE);
	_REGISTER_CONSTANT(CL_INVALID_SAMPLER);
	_REGISTER_CONSTANT(CL_INVALID_BINARY);
	_REGISTER_CONSTANT(CL_INVALID_BUILD_OPTIONS);
	_REGISTER_CONSTANT(CL_INVALID_PROGRAM);
	_REGISTER_CONSTANT(CL_INVALID_PROGRAM_EXECUTABLE);
	_REGISTER_CONSTANT(CL_INVALID_KERNEL_NAME);
	_REGISTER_CONSTANT(CL_INVALID_KERNEL_DEFINITION);
	_REGISTER_CONSTANT(CL_INVALID_KERNEL);
	_REGISTER_CONSTANT(CL_INVALID_ARG_INDEX);
	_REGISTER_CONSTANT(CL_INVALID_ARG_VALUE);
	_REGISTER_CONSTANT(CL_INVALID_ARG_SIZE);
	_REGISTER_CONSTANT(CL_INVALID_KERNEL_ARGS);
	_REGISTER_CONSTANT(CL_INVALID_WORK_DIMENSION);
	_REGISTER_CONSTANT(CL_INVALID_WORK_GROUP_SIZE);
	_REGISTER_CONSTANT(CL_INVALID_WORK_ITEM_SIZE);
	_REGISTER_CONSTANT(CL_INVALID_GLOBAL_OFFSET);
	_REGISTER_CONSTANT(CL_INVALID_EVENT_WAIT_LIST);
	_REGISTER_CONSTANT(CL_INVALID_EVENT);
	_REGISTER_CONSTANT(CL_INVALID_OPERATION);
	_REGISTER_CONSTANT(CL_INVALID_GL_OBJECT);
	_REGISTER_CONSTANT(CL_INVALID_BUFFER_SIZE);
	_REGISTER_CONSTANT(CL_INVALID_MIP_LEVEL);
	_REGISTER_CONSTANT(CL_INVALID_GLOBAL_WORK_SIZE);
	_REGISTER_CONSTANT(CL_INVALID_PROPERTY);

	/* OpenCL Version */
	_REGISTER_CONSTANT(CL_VERSION_1_0);
#ifdef CL_VERSION_1_1
	_REGISTER_CONSTANT(CL_VERSION_1_1);
#endif

	/* cl_bool */
	_REGISTER_CONSTANT(CL_FALSE);
	_REGISTER_CONSTANT(CL_TRUE);
	_REGISTER_CONSTANT(CL_BLOCKING);
	_REGISTER_CONSTANT(CL_NON_BLOCKING);

	/* cl_platform_info */
	_REGISTER_CONSTANT(CL_PLATFORM_PROFILE);
	_REGISTER_CONSTANT(CL_PLATFORM_VERSION);
	_REGISTER_CONSTANT(CL_PLATFORM_NAME);
	_REGISTER_CONSTANT(CL_PLATFORM_VENDOR);
	_REGISTER_CONSTANT(CL_PLATFORM_EXTENSIONS);

	/* cl_device_type - bitfield */
	_REGISTER_CONSTANT(CL_DEVICE_TYPE_DEFAULT);
	_REGISTER_CONSTANT(CL_DEVICE_TYPE_CPU);
	_REGISTER_CONSTANT(CL_DEVICE_TYPE_GPU);
	_REGISTER_CONSTANT(CL_DEVICE_TYPE_ACCELERATOR);
	_REGISTER_CONSTANT(CL_DEVICE_TYPE_ALL);

	/* cl_device_info */
	_REGISTER_CONSTANT(CL_DEVICE_TYPE);
	_REGISTER_CONSTANT(CL_DEVICE_VENDOR_ID);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_COMPUTE_UNITS);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_WORK_GROUP_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_WORK_ITEM_SIZES);
	_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
	_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
	_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
	_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
	_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
	_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_CLOCK_FREQUENCY);
	_REGISTER_CONSTANT(CL_DEVICE_ADDRESS_BITS);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_READ_IMAGE_ARGS);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_MEM_ALLOC_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_IMAGE2D_MAX_WIDTH);
	_REGISTER_CONSTANT(CL_DEVICE_IMAGE2D_MAX_HEIGHT);
	_REGISTER_CONSTANT(CL_DEVICE_IMAGE3D_MAX_WIDTH);
	_REGISTER_CONSTANT(CL_DEVICE_IMAGE3D_MAX_HEIGHT);
	_REGISTER_CONSTANT(CL_DEVICE_IMAGE3D_MAX_DEPTH);
	_REGISTER_CONSTANT(CL_DEVICE_IMAGE_SUPPORT);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_PARAMETER_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_SAMPLERS);
	_REGISTER_CONSTANT(CL_DEVICE_MEM_BASE_ADDR_ALIGN);
	_REGISTER_CONSTANT(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_SINGLE_FP_CONFIG);
	_REGISTER_CONSTANT(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE);
	_REGISTER_CONSTANT(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_GLOBAL_MEM_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_MAX_CONSTANT_ARGS);
	_REGISTER_CONSTANT(CL_DEVICE_LOCAL_MEM_TYPE);
	_REGISTER_CONSTANT(CL_DEVICE_LOCAL_MEM_SIZE);
	_REGISTER_CONSTANT(CL_DEVICE_ERROR_CORRECTION_SUPPORT);
	_REGISTER_CONSTANT(CL_DEVICE_PROFILING_TIMER_RESOLUTION);
	_REGISTER_CONSTANT(CL_DEVICE_ENDIAN_LITTLE);
	_REGISTER_CONSTANT(CL_DEVICE_AVAILABLE);
	_REGISTER_CONSTANT(CL_DEVICE_COMPILER_AVAILABLE);
	_REGISTER_CONSTANT(CL_DEVICE_EXECUTION_CAPABILITIES);
	_REGISTER_CONSTANT(CL_DEVICE_QUEUE_PROPERTIES);
	_REGISTER_CONSTANT(CL_DEVICE_NAME);
	_REGISTER_CONSTANT(CL_DEVICE_VENDOR);
	_REGISTER_CONSTANT(CL_DRIVER_VERSION);
	_REGISTER_CONSTANT(CL_DEVICE_PROFILE);
	_REGISTER_CONSTANT(CL_DEVICE_VERSION);
	_REGISTER_CONSTANT(CL_DEVICE_EXTENSIONS);
	_REGISTER_CONSTANT(CL_DEVICE_PLATFORM);
#ifdef CL_DEVICE_DOUBLE_FP_CONFIG
	_REGISTER_CONSTANT(CL_DEVICE_DOUBLE_FP_CONFIG);
#endif
#ifdef CL_DEVICE_HALF_FP_CONFIG
	_REGISTER_CONSTANT(CL_DEVICE_HALF_FP_CONFIG);
#endif
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF
	_REGISTER_CONSTANT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF);
#endif
#ifdef CL_DEVICE_HOST_UNIFIED_MEMORY
	_REGISTER_CONSTANT(CL_DEVICE_HOST_UNIFIED_MEMORY);
#endif
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR
	_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR);
	_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT);
	_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_INT);
	_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG);
	_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT);
	_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE);
	_REGISTER_CONSTANT(CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF);
#endif
#ifdef CL_DEVICE_OPENCL_C_VERSION
	_REGISTER_CONSTANT(CL_DEVICE_OPENCL_C_VERSION);
#endif

	/* cl_device_fp_config - bitfield */
	_REGISTER_CONSTANT(CL_FP_DENORM);
	_REGISTER_CONSTANT(CL_FP_INF_NAN);
	_REGISTER_CONSTANT(CL_FP_ROUND_TO_NEAREST);
	_REGISTER_CONSTANT(CL_FP_ROUND_TO_ZERO);
	_REGISTER_CONSTANT(CL_FP_ROUND_TO_INF);
	_REGISTER_CONSTANT(CL_FP_FMA);
	_REGISTER_CONSTANT(CL_FP_SOFT_FLOAT);

	/* cl_device_mem_cache_type */
	_REGISTER_CONSTANT(CL_NONE);
	_REGISTER_CONSTANT(CL_READ_ONLY_CACHE);
	_REGISTER_CONSTANT(CL_READ_WRITE_CACHE);

	/* cl_device_local_mem_type */
	_REGISTER_CONSTANT(CL_LOCAL);
	_REGISTER_CONSTANT(CL_GLOBAL);

	/* cl_device_exec_capabilities - bitfield */
	_REGISTER_CONSTANT(CL_EXEC_KERNEL);
	_REGISTER_CONSTANT(CL_EXEC_NATIVE_KERNEL);

	/* cl_command_queue_properties - bitfield */
	_REGISTER_CONSTANT(CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
	_REGISTER_CONSTANT(CL_QUEUE_PROFILING_ENABLE);

	/* cl_context_info  */
	_REGISTER_CONSTANT(CL_CONTEXT_REFERENCE_COUNT);
	_REGISTER_CONSTANT(CL_CONTEXT_DEVICES);
	_REGISTER_CONSTANT(CL_CONTEXT_PROPERTIES);
	_REGISTER_CONSTANT(CL_CONTEXT_NUM_DEVICES);

	/* cl_context_info + cl_context_properties */
	_REGISTER_CONSTANT(CL_CONTEXT_PLATFORM);

	/* cl_command_queue_info */
	_REGISTER_CONSTANT(CL_QUEUE_CONTEXT);
	_REGISTER_CONSTANT(CL_QUEUE_DEVICE);
	_REGISTER_CONSTANT(CL_QUEUE_REFERENCE_COUNT);
	_REGISTER_CONSTANT(CL_QUEUE_PROPERTIES);

	/* cl_mem_flags - bitfield */
	_REGISTER_CONSTANT(CL_MEM_READ_WRITE);
	_REGISTER_CONSTANT(CL_MEM_WRITE_ONLY);
	_REGISTER_CONSTANT(CL_MEM_READ_ONLY);
	_REGISTER_CONSTANT(CL_MEM_USE_HOST_PTR);
	_REGISTER_CONSTANT(CL_MEM_ALLOC_HOST_PTR);
	_REGISTER_CONSTANT(CL_MEM_COPY_HOST_PTR);

	/* cl_channel_order */
	_REGISTER_CONSTANT(CL_R);
	_REGISTER_CONSTANT(CL_A);
	_REGISTER_CONSTANT(CL_RG);
	_REGISTER_CONSTANT(CL_RA);
	_REGISTER_CONSTANT(CL_RGB);
	_REGISTER_CONSTANT(CL_RGBA);
	_REGISTER_CONSTANT(CL_BGRA);
	_REGISTER_CONSTANT(CL_ARGB);
	_REGISTER_CONSTANT(CL_INTENSITY);
	_REGISTER_CONSTANT(CL_LUMINANCE);
	_REGISTER_CONSTANT(CL_Rx);
	_REGISTER_CONSTANT(CL_RGx);
	_REGISTER_CONSTANT(CL_RGBx);

	/* cl_channel_type */
	_REGISTER_CONSTANT(CL_SNORM_INT8);
	_REGISTER_CONSTANT(CL_SNORM_INT16);
	_REGISTER_CONSTANT(CL_UNORM_INT8);
	_REGISTER_CONSTANT(CL_UNORM_INT16);
	_REGISTER_CONSTANT(CL_UNORM_SHORT_565);
	_REGISTER_CONSTANT(CL_UNORM_SHORT_555);
	_REGISTER_CONSTANT(CL_UNORM_INT_101010);
	_REGISTER_CONSTANT(CL_SIGNED_INT8);
	_REGISTER_CONSTANT(CL_SIGNED_INT16);
	_REGISTER_CONSTANT(CL_SIGNED_INT32);
	_REGISTER_CONSTANT(CL_UNSIGNED_INT8);
	_REGISTER_CONSTANT(CL_UNSIGNED_INT16);
	_REGISTER_CONSTANT(CL_UNSIGNED_INT32);
	_REGISTER_CONSTANT(CL_HALF_FLOAT);
	_REGISTER_CONSTANT(CL_FLOAT);

	/* cl_mem_object_type */
	_REGISTER_CONSTANT(CL_MEM_OBJECT_BUFFER);
	_REGISTER_CONSTANT(CL_MEM_OBJECT_IMAGE2D);
	_REGISTER_CONSTANT(CL_MEM_OBJECT_IMAGE3D);

	/* cl_mem_info */
	_REGISTER_CONSTANT(CL_MEM_TYPE);
	_REGISTER_CONSTANT(CL_MEM_FLAGS);
	_REGISTER_CONSTANT(CL_MEM_SIZE);
	_REGISTER_CONSTANT(CL_MEM_HOST_PTR);
	_REGISTER_CONSTANT(CL_MEM_MAP_COUNT);
	_REGISTER_CONSTANT(CL_MEM_REFERENCE_COUNT);
	_REGISTER_CONSTANT(CL_MEM_CONTEXT);
	_REGISTER_CONSTANT(CL_MEM_ASSOCIATED_MEMOBJECT);
	_REGISTER_CONSTANT(CL_MEM_OFFSET);

	/* cl_image_info */
	_REGISTER_CONSTANT(CL_IMAGE_FORMAT);
	_REGISTER_CONSTANT(CL_IMAGE_ELEMENT_SIZE);
	_REGISTER_CONSTANT(CL_IMAGE_ROW_PITCH);
	_REGISTER_CONSTANT(CL_IMAGE_SLICE_PITCH);
	_REGISTER_CONSTANT(CL_IMAGE_WIDTH);
	_REGISTER_CONSTANT(CL_IMAGE_HEIGHT);
	_REGISTER_CONSTANT(CL_IMAGE_DEPTH);

	/* cl_addressing_mode */
	_REGISTER_CONSTANT(CL_ADDRESS_NONE);
	_REGISTER_CONSTANT(CL_ADDRESS_CLAMP_TO_EDGE);
	_REGISTER_CONSTANT(CL_ADDRESS_CLAMP);
	_REGISTER_CONSTANT(CL_ADDRESS_REPEAT);
	_REGISTER_CONSTANT(CL_ADDRESS_MIRRORED_REPEAT);

	/* cl_filter_mode */
	_REGISTER_CONSTANT(CL_FILTER_NEAREST);
	_REGISTER_CONSTANT(CL_FILTER_LINEAR);

	/* cl_sampler_info */
	_REGISTER_CONSTANT(CL_SAMPLER_REFERENCE_COUNT);
	_REGISTER_CONSTANT(CL_SAMPLER_CONTEXT);
	_REGISTER_CONSTANT(CL_SAMPLER_NORMALIZED_COORDS);
	_REGISTER_CONSTANT(CL_SAMPLER_ADDRESSING_MODE);
	_REGISTER_CONSTANT(CL_SAMPLER_FILTER_MODE);

	/* cl_map_flags - bitfield */
	_REGISTER_CONSTANT(CL_MAP_READ);
	_REGISTER_CONSTANT(CL_MAP_WRITE);

	/* cl_program_info */
	_REGISTER_CONSTANT(CL_PROGRAM_REFERENCE_COUNT);
	_REGISTER_CONSTANT(CL_PROGRAM_CONTEXT);
	_REGISTER_CONSTANT(CL_PROGRAM_NUM_DEVICES);
	_REGISTER_CONSTANT(CL_PROGRAM_DEVICES);
	_REGISTER_CONSTANT(CL_PROGRAM_SOURCE);
	_REGISTER_CONSTANT(CL_PROGRAM_BINARY_SIZES);
	_REGISTER_CONSTANT(CL_PROGRAM_BINARIES);

	/* cl_program_build_info */
	_REGISTER_CONSTANT(CL_PROGRAM_BUILD_STATUS);
	_REGISTER_CONSTANT(CL_PROGRAM_BUILD_OPTIONS);
	_REGISTER_CONSTANT(CL_PROGRAM_BUILD_LOG);

	/* cl_build_status */
	_REGISTER_CONSTANT(CL_BUILD_SUCCESS);
	_REGISTER_CONSTANT(CL_BUILD_NONE);
	_REGISTER_CONSTANT(CL_BUILD_ERROR);
	_REGISTER_CONSTANT(CL_BUILD_IN_PROGRESS);

	/* cl_kernel_info */
	_REGISTER_CONSTANT(CL_KERNEL_FUNCTION_NAME);
	_REGISTER_CONSTANT(CL_KERNEL_NUM_ARGS);
	_REGISTER_CONSTANT(CL_KERNEL_REFERENCE_COUNT);
	_REGISTER_CONSTANT(CL_KERNEL_CONTEXT);
	_REGISTER_CONSTANT(CL_KERNEL_PROGRAM);

	/* cl_kernel_work_group_info */
	_REGISTER_CONSTANT(CL_KERNEL_WORK_GROUP_SIZE);
	_REGISTER_CONSTANT(CL_KERNEL_COMPILE_WORK_GROUP_SIZE);
	_REGISTER_CONSTANT(CL_KERNEL_LOCAL_MEM_SIZE);
	_REGISTER_CONSTANT(CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE);
	_REGISTER_CONSTANT(CL_KERNEL_PRIVATE_MEM_SIZE);

	/* cl_event_info  */
	_REGISTER_CONSTANT(CL_EVENT_COMMAND_QUEUE);
	_REGISTER_CONSTANT(CL_EVENT_COMMAND_TYPE);
	_REGISTER_CONSTANT(CL_EVENT_REFERENCE_COUNT);
	_REGISTER_CONSTANT(CL_EVENT_COMMAND_EXECUTION_STATUS);
	_REGISTER_CONSTANT(CL_EVENT_CONTEXT);

	/* cl_command_type */
	_REGISTER_CONSTANT(CL_COMMAND_NDRANGE_KERNEL);
	_REGISTER_CONSTANT(CL_COMMAND_TASK);
	_REGISTER_CONSTANT(CL_COMMAND_NATIVE_KERNEL);
	_REGISTER_CONSTANT(CL_COMMAND_READ_BUFFER);
	_REGISTER_CONSTANT(CL_COMMAND_WRITE_BUFFER);
	_REGISTER_CONSTANT(CL_COMMAND_COPY_BUFFER);
	_REGISTER_CONSTANT(CL_COMMAND_READ_IMAGE);
	_REGISTER_CONSTANT(CL_COMMAND_WRITE_IMAGE);
	_REGISTER_CONSTANT(CL_COMMAND_COPY_IMAGE);
	_REGISTER_CONSTANT(CL_COMMAND_COPY_IMAGE_TO_BUFFER);
	_REGISTER_CONSTANT(CL_COMMAND_COPY_BUFFER_TO_IMAGE);
	_REGISTER_CONSTANT(CL_COMMAND_MAP_BUFFER);
	_REGISTER_CONSTANT(CL_COMMAND_MAP_IMAGE);
	_REGISTER_CONSTANT(CL_COMMAND_UNMAP_MEM_OBJECT);
	_REGISTER_CONSTANT(CL_COMMAND_MARKER);
	_REGISTER_CONSTANT(CL_COMMAND_ACQUIRE_GL_OBJECTS);
	_REGISTER_CONSTANT(CL_COMMAND_RELEASE_GL_OBJECTS);
	_REGISTER_CONSTANT(CL_COMMAND_READ_BUFFER_RECT);
	_REGISTER_CONSTANT(CL_COMMAND_WRITE_BUFFER_RECT);
	_REGISTER_CONSTANT(CL_COMMAND_COPY_BUFFER_RECT);
	_REGISTER_CONSTANT(CL_COMMAND_USER);

	/* command execution status */
	_REGISTER_CONSTANT(CL_COMPLETE);
	_REGISTER_CONSTANT(CL_RUNNING);
	_REGISTER_CONSTANT(CL_SUBMITTED);
	_REGISTER_CONSTANT(CL_QUEUED);

	/* cl_buffer_create_type  */
	_REGISTER_CONSTANT(CL_BUFFER_CREATE_TYPE_REGION);

	/* cl_profiling_info  */
	_REGISTER_CONSTANT(CL_PROFILING_COMMAND_QUEUED);
	_REGISTER_CONSTANT(CL_PROFILING_COMMAND_SUBMIT);
	_REGISTER_CONSTANT(CL_PROFILING_COMMAND_START);
	_REGISTER_CONSTANT(CL_PROFILING_COMMAND_END);
}

/* }}} */
/* {{{ phpcl_errstr() */

const char *phpcl_errstr(cl_int errcode)
{
	switch(errcode) {
	case CL_SUCCESS:
		return "SUCCESS";
	case CL_DEVICE_NOT_FOUND:
		return "DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
		return "DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
		return "COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
		return "OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
		return "OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return "PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
		return "MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
		return "IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return "IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
		return "BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
		return "MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		return "MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return "EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_INVALID_VALUE:
		return "INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
		return "INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return "INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return "INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return "INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return "INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
		return "INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
		return "INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
		return "INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return "INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return "INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return "INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return "INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return "INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:
		return "INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return "INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
		return "INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
		return "INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
		return "INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return "INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return "INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
		return "INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
		return "INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
		return "INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
		return "INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
		return "INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
		return "INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
		return "INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return "INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return "INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
		return "INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
		return "INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
		return "INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:
		return "INVALID_GLOBAL_WORK_SIZE";
	case CL_INVALID_PROPERTY:
		return "INVALID_PROPERTY";
	}
	return "UNKNOWN";
}

/* }}} */
/* {{{ resource id functions */

int phpcl_le_platform(void)
{
	return le_platform;
}

int phpcl_le_device(void)
{
	return le_device;
}

int phpcl_le_context(void)
{
	return le_context;
}

int phpcl_le_command_queue(void)
{
	return le_command_queue;
}

int phpcl_le_mem(void)
{
	return le_mem;
}

int phpcl_le_event(void)
{
	return le_event;
}

int phpcl_le_program(void)
{
	return le_program;
}

int phpcl_le_kernel(void)
{
	return le_kernel;
}

int phpcl_le_sampler(void)
{
	return le_sampler;
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
