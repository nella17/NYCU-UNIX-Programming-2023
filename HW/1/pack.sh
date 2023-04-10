#!/bin/bash
tar cvf "${ID:=pack}.tar.gz" Makefile sandbox.{c,h}pp
