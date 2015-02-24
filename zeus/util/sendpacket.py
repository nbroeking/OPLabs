#!/usr/bin/env python
"""

Demo util for sending a single wakeup packet to the router.

Author: Zach Anders
Date: 2/23/15

"""

from common.router import Router
import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: ./sendpacket <address>")
    router = Router(sys.argv[1])
    router.wakeup()
