/*
  This file is part of Ingen.
  Copyright 2007-2012 David Robillard <http://drobilla.net/>

  Ingen is free software: you can redistribute it and/or modify it under the
  terms of the GNU Affero General Public License as published by the Free
  Software Foundation, either version 3 of the License, or any later version.

  Ingen is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Affero General Public License for details.

  You should have received a copy of the GNU Affero General Public License
  along with Ingen.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ELEMENT_COMPILED_GRAPH_HPP
#define ELEMENT_COMPILED_GRAPH_HPP

#include "block-node.hpp"

namespace element {

class CompiledBlock
{
public:

    CompiledBlock (BlockNode* blk, const Array<BlockNode*>& deps)
        : p_block (blk)
	{
		// Copy to a vector for maximum iteration speed and cache optimization
        m_deps.reserve (deps.size());
		for (const auto& d : deps)
            m_deps.push_back (d);
	}

    BlockNode*                     block()       const { return p_block; }
    const std::vector<BlockNode*>& dependents()  const { return m_deps; }

private:

    BlockNode*              p_block;
    std::vector<BlockNode*> m_deps; ///< Blocks this one's output ports are connected to

};

class CompiledGraph : public std::vector<CompiledBlock>
{ };

} /* namespace element */

#endif // ELEMENT_COMPILED_GRAPH_HPP
