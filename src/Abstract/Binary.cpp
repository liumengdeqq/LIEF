/* Copyright 2017 R. Thomas
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "LIEF/Abstract/Binary.hpp"
#include "LIEF/exception.hpp"

#include "LIEF/ELF/Binary.hpp"
#include "LIEF/PE/Binary.hpp"
#include "LIEF/MachO/Binary.hpp"

namespace LIEF {
Binary::Binary(void) :
  name_{""},
  original_size_{0}
{}

Binary::~Binary(void) = default;
Binary& Binary::operator=(const Binary&) = default;
Binary::Binary(const Binary&) = default;

EXE_FORMATS Binary::format(void) const {
  if (typeid(*this) == typeid(LIEF::ELF::Binary)) {
    return EXE_FORMATS::FORMAT_ELF;
  }

  if (typeid(*this) == typeid(LIEF::PE::Binary)) {
    return EXE_FORMATS::FORMAT_PE;
  }

  if (typeid(*this) == typeid(LIEF::MachO::Binary)) {
    return EXE_FORMATS::FORMAT_MACHO;
  }

  return EXE_FORMATS::FORMAT_UNKNOWN;
}

Header Binary::get_header(void) const {
  return this->get_abstract_header();
}

it_symbols Binary::get_symbols(void) {
  return it_symbols{this->get_abstract_symbols()};
}

it_const_symbols Binary::get_symbols(void) const {
  return it_const_symbols{const_cast<Binary*>(this)->get_abstract_symbols()};
}


bool Binary::has_symbol(const std::string& name) const {
  symbols_t symbols = const_cast<Binary*>(this)->get_abstract_symbols();
  auto&& it_symbol = std::find_if(
      std::begin(symbols),
      std::end(symbols),
      [&name] (const Symbol* s) {
        return s->name() == name;
      });

  return it_symbol != std::end(symbols);
}

const Symbol& Binary::get_symbol(const std::string& name) const {
  if (not this->has_symbol(name)) {
    throw not_found("Symbol '" + name + "' not found!");
  }

  symbols_t symbols = const_cast<Binary*>(this)->get_abstract_symbols();

  auto&& it_symbol = std::find_if(
      std::begin(symbols),
      std::end(symbols),
      [&name] (const Symbol* s) {
        return s->name() == name;
      });

  return **it_symbol;
}

Symbol& Binary::get_symbol(const std::string& name) {
  return const_cast<Symbol&>(static_cast<const Binary*>(this)->get_symbol(name));
}

it_sections Binary::get_sections(void) {
  return it_sections{this->get_abstract_sections()};
}


it_const_sections Binary::get_sections(void) const {
  return it_const_sections{const_cast<Binary*>(this)->get_abstract_sections()};
}


std::vector<std::string> Binary::get_exported_functions(void) const {
  return this->get_abstract_exported_functions();
}

std::vector<std::string> Binary::get_imported_functions(void) const {
  return this->get_abstract_imported_functions();
}


std::vector<std::string> Binary::get_imported_libraries(void) const {
  return this->get_abstract_imported_libraries();
}

uint64_t Binary::get_function_address(const std::string&) const {
  throw not_implemented("Not implemented for this format");
}

std::vector<uint64_t> Binary::xref(uint64_t address) const {
  std::vector<uint64_t> result;

  for (Section* section : const_cast<Binary*>(this)->get_abstract_sections()) {
    std::vector<size_t> founds = section->search_all(address);
    for (size_t found : founds) {
      result.push_back(section->virtual_address() + found);
    }
  }

  return result;
}

void Binary::accept(Visitor& visitor) const {
  visitor(this->get_header());
  for (const Section& section : const_cast<Binary*>(this)->get_sections()) {
    visitor(section);
  }


  for (const Symbol& symbol : const_cast<Binary*>(this)->get_symbols()) {
    visitor(symbol);
  }
}

const std::string& Binary::name(void) const {
  return this->name_;
}


uint64_t Binary::original_size(void) const {
  return this->original_size_;
}


void Binary::name(const std::string& name) {
  this->name_ = name;
}


void Binary::original_size(uint64_t size) {
  this->original_size_ = size;
}


std::ostream& Binary::print(std::ostream& os) const {
  return os;
}

std::ostream& operator<<(std::ostream& os, const Binary& binary) {
  binary.print(os);
  return os;
}


}
