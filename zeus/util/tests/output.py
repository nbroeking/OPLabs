__COL_GOOD = '\033[32m'
__COL_FAIL = '\033[31m'
__COL_INFO = '\033[34m'
__COL_BOLD = '\033[1m'
__COL_ULIN = '\033[4m'
__COL_ENDC = '\033[0m'

def __TEST__(status, msg, color, args):
    args = ", ".join([str(key)+'='+str(args[key]) for key in args.keys()])
    if args:
        args = "(" + args + ")"
    return "[{color}{status}{end}] {msg} {args}".format(
        color=color,
        status=status,
        end=__COL_ENDC,
        msg=msg,
        args=args
        )

def SUCCESS(test_name, **kwargs):
    msg = "Test {tname} passed.".format(tname=test_name)
    return __TEST__('PASS', msg, __COL_GOOD, kwargs)

def FAILURE(test_name, **kwargs):
    msg = "Test {tname} failed.".format(tname=test_name)
    return __TEST__('FAIL', msg, __COL_FAIL, kwargs)

def ANSI_wrapper(prefix):
    def inner(message):
        return prefix + message + __COL_ENDC
    return inner

def truncate_repr(val, priority=None):
    if priority and isinstance(val, dict):
        val_copy = dict(val)
        output = '{'
        for k, v in priority.items():
            output += "%s, %s" % (k, v)
            val_copy.pop(k)
        output += ", " + str(val_copy)[1:]
    else:
        output = str(val)
    if len(output) <= 64:
        return output
    output = output[:64]
    if isinstance(val, dict):
        output += "...}"
    elif isinstance(val, list):
        output += "...]"
    return output

INFO = ANSI_wrapper(__COL_INFO)
BOLD = ANSI_wrapper(__COL_BOLD)
UNDERLINE = ANSI_wrapper(__COL_ULIN)
