#ifndef ERROR_ERROR_CODES_H
#define ERROR_ERROR_CODES_H

namespace accord {

enum Error {
    ERR_NAN = 0,
	NOT_FOUND = 1,
    TOO_LONG = 2,
    AUTH_ERR = 3,
    ARGS_ERR = 4,
    REGIST_ERR = 5,
    REQUEST_ERR = 6,
    FORBIDDEN_ERR = 7,
};

} /* namespace accord */

#endif
