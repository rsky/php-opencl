/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "php_opencl.h"

#ifndef PHPCL_PLATFORM_H
#define PHPCL_PLATFORM_H
BEGIN_EXTERN_C()

PHPCL_FUNCTION(cl_get_platform_ids);
PHPCL_FUNCTION(cl_get_platform_info);

END_EXTERN_C()
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
