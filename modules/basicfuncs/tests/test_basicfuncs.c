/*
 * Copyright (c) 2010-2015 Balabit
 * Copyright (c) 2010-2015 Balázs Scheidler
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

#include "template_lib.h"
#include "apphook.h"
#include "plugin.h"

void
test_cond_funcs(void)
{
  assert_template_format_with_context("$(grep 'facility(local3)' $PID)", "23323,23323");
  assert_template_format_with_context("$(grep 'facility(local3)' $PID $PROGRAM)", "23323,syslog-ng,23323,syslog-ng");
  assert_template_format_with_context("$(grep 'facility(local4)' $PID)", "");
  assert_template_format_with_context("$(grep ('$FACILITY' == 'local4') $PID)", "");
  assert_template_format_with_context("$(grep ('$FACILITY(' == 'local3(') $PID)", "23323,23323");
  assert_template_format_with_context("$(grep ('$FACILITY(' == 'local4)') $PID)", "");
  assert_template_format_with_context("$(grep \\'$FACILITY\\'\\ ==\\ \\'local4\\' $PID)", "");

  assert_template_format_with_context("$(if 'facility(local4)' alma korte)", "korte");
  assert_template_format_with_context("$(if 'facility(local3)' alma korte)", "alma");

  assert_template_format_with_context("$(if '\"$FACILITY\" lt \"local3\"' alma korte)", "korte");
  assert_template_format_with_context("$(if '\"$FACILITY\" le \"local3\"' alma korte)", "alma");
  assert_template_format_with_context("$(if '\"$FACILITY\" eq \"local3\"' alma korte)", "alma");
  assert_template_format_with_context("$(if '\"$FACILITY\" ne \"local3\"' alma korte)", "korte");
  assert_template_format_with_context("$(if '\"$FACILITY\" gt \"local3\"' alma korte)", "korte");
  assert_template_format_with_context("$(if '\"$FACILITY\" ge \"local3\"' alma korte)", "alma");

  assert_template_format_with_context("$(if '\"$FACILITY_NUM\" < \"19\"' alma korte)", "korte");
  assert_template_format_with_context("$(if '\"$FACILITY_NUM\" <= \"19\"' alma korte)", "alma");
  assert_template_format_with_context("$(if '\"$FACILITY_NUM\" == \"19\"' alma korte)", "alma");
  assert_template_format_with_context("$(if '\"$FACILITY_NUM\" != \"19\"' alma korte)", "korte");
  assert_template_format_with_context("$(if '\"$FACILITY_NUM\" > \"19\"' alma korte)", "korte");
  assert_template_format_with_context("$(if '\"$FACILITY_NUM\" >= \"19\"' alma korte)", "alma");
  assert_template_format_with_context("$(if '\"$FACILITY_NUM\" >= \"19\" and \"kicsi\" == \"nagy\"' alma korte)", "korte");
  assert_template_format_with_context("$(if '\"$FACILITY_NUM\" >= \"19\" or \"kicsi\" == \"nagy\"' alma korte)", "alma");

  assert_template_format_with_context("$(grep 'facility(local3)' $PID)@0", "23323");
  assert_template_format_with_context("$(grep 'facility(local3)' $PID)@1", "23323");
  assert_template_format_with_context("$(grep 'facility(local3)' $PID)@2", "");
}

void
test_str_funcs(void)
{
  assert_template_format("$(strip ${APP.STRIP1})", "value");
  assert_template_format("$(strip ${APP.STRIP2})", "value");
  assert_template_format("$(strip ${APP.STRIP3})", "value");
  assert_template_format("$(strip ${APP.STRIP4})", "value");
  assert_template_format("$(strip ${APP.STRIP5})", "");

  assert_template_format("$(strip ${APP.STRIP1} ${APP.STRIP2} ${APP.STRIP3} ${APP.STRIP4} ${APP.STRIP5})", "value value value value");
}

void
test_numeric_funcs(void)
{
  assert_template_format("$(+ $FACILITY_NUM 1)", "20");
  assert_template_format("$(+ -1 -1)", "-2");
  assert_template_format("$(- $FACILITY_NUM 1)", "18");
  assert_template_format("$(- $FACILITY_NUM 20)", "-1");
  assert_template_format("$(* $FACILITY_NUM 2)", "38");
  assert_template_format("$(/ $FACILITY_NUM 2)", "9");
  assert_template_format("$(% $FACILITY_NUM 3)", "1");
  assert_template_format("$(/ $FACILITY_NUM 0)", "NaN");
  assert_template_format("$(% $FACILITY_NUM 0)", "NaN");
  assert_template_format("$(+ foo bar)", "NaN");
}

int
main(int argc G_GNUC_UNUSED, char *argv[] G_GNUC_UNUSED)
{
  app_startup();
  init_template_tests();
  plugin_load_module("basicfuncs", configuration, NULL);

  test_cond_funcs();
  test_str_funcs();

  deinit_template_tests();
  app_shutdown();
}
