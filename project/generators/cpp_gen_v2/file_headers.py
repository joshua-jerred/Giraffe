import datetime

def getCurrentDate():
    return datetime.datetime.now().strftime("%Y-%m-%d")


AUTO_GEN_HEADER = f"""/**
 *
 *
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 *
 *
 * =*========GIRAFFE========*=
 * A Unified Flight Command and Control System
 * https://github.com/joshua-jerred/Giraffe
 * https://giraffe.joshuajer.red/
 * =*=======================*=
 *
 *
 * =*=======================*=
 * @author     Joshua Jerred (https://joshuajer.red)
 * @date       {getCurrentDate()}
 * @copyright  2023 (license to be defined)
 * =*=======================*=
 *
 * @verbatim
 */
"""

AUTO_GEN_FOOTER = f"""
/**
 *
 * Generated on: {getCurrentDate()}
 *
 * @endverbatim
 *
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 * AUTOMATICALLY GENERATED, DO NOT EDIT MANUALLY
 * &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 */"""