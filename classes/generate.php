<?php
if (!extension_loaded('opencl')) {
    echo "opencl extension is not loaded!\n";
    exit(1);
}

chdir(__DIR__);
$skeleton = file_get_contents('Cl.php.in');
if (!$skeleton) {
    exit(1);
}

$reflector = new ReflectionExtension('opencl');

$constDefinitions = array();
$constExceptions = array(
    'CL_GLOBAL' => 'MEM_TYPE_GLOBAL',
    'CL_LOCAL'  => 'MEM_TYPE_LOCAL',
    'CL_TRUE'   => null,
    'CL_FALSE'  => null,
    'CL_VERSION_1_0'    => null,
    'CL_VERSION_1_1'    => null,
    'CL_VERSION_1_2'    => null,
);

foreach ($reflector->getConstants() as $name => $value) {
    if (array_key_exists($name, $constExceptions)) {
        $name = $constExceptions[$name];
        if (is_null($name)) {
            continue;
        }
    } else {
        $name = substr($name, 3);
    }
    $constDefinitions[] = sprintf('const %s = %s;', $name,
                                  var_export($value, true));
}

$source = str_replace('/* CONSTANTS */',
                      implode("\n    ", $constDefinitions),
                      $skeleton);
file_put_contents('Cl.php', $source);
