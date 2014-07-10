/*
 * codbg_target_if.h
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#ifndef CODBG_TARGET_IF_H_
#define CODBG_TARGET_IF_H_
class codbg_agent_if;

class codbg_target_if {

	public:

		virtual ~codbg_target_if() {}

		virtual void init(
				codbg_agent_if *agent_if) = 0;


	private:


};




#endif /* CODBG_TARGET_IF_H_ */
