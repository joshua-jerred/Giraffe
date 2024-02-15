import { useEffect, useContext, useState } from "react";

import { StyTable } from "./styled/StyledTable";
import { useTable } from "react-table";

import { GGS_API } from "../api_interface/ws_api";
import { GwsGlobal } from "../GlobalContext";
import {
  StyButton,
  StyInput,
  Switch,
  StySelect,
  StyOption,
} from "./styled/StyledComponents";

function TableCell(props) {
  // console.log(props);
  if (props.metadata.type === "bool") {
    return <td>{props.value === true ? "true" : "false"}</td>;
  }
  return <td>{props.children}</td>;
}

function Table({ columns, data, editmode }) {
  const columnPropGetter = (meta) => ({ metadata: meta });
  const cellPropGetter = (data) => ({ value: data.value });
  const {
    getTableProps,
    getTableBodyProps,
    headerGroups,
    rows,
    prepareRow,
    getColumnProps = columnPropGetter,
    getCellProps = cellPropGetter,
  } = useTable({
    columns,
    data,
  });
  return (
    <StyTable {...getTableProps()}>
      <thead>
        {headerGroups.map((headerGroup) => (
          <tr {...headerGroup.getHeaderGroupProps()}>
            {headerGroup.headers.map((column) => (
              <th {...column.getHeaderProps()}>{column.render("name")}</th>
            ))}
          </tr>
        ))}
      </thead>
      <tbody {...getTableBodyProps()}>
        {rows.map((row, i) => {
          prepareRow(row);
          return (
            <tr {...row.getRowProps()}>
              {row.cells.map((cell) => {
                return (
                  <TableCell
                    // Merge in type props
                    {...cell.getCellProps([
                      {
                        editmode: editmode,
                      },
                      getColumnProps(cell.column),
                      getCellProps(cell),
                    ])}
                  >
                    {cell.render("Cell")}
                  </TableCell>
                );
              })}
            </tr>
          );
        })}
      </tbody>
    </StyTable>
  );
}

export default function EditableTable({ resource, category }) {
  const { ggsAddress } = useContext(GwsGlobal);
  const { ggsConnectionStatus } = useContext(GGS_API);
  const [metadata, setMetadata] = useState(null);
  const [values, setValues] = useState(null);
  const [error, setError] = useState(null);
  const [editMode, setEditMode] = useState(false);

  useEffect(() => {
    const encoded_category = encodeURIComponent(category);
    const path = `http://${ggsAddress}/api/${resource}/settings?category=${encoded_category}&include=all`;

    if (ggsConnectionStatus !== "connected") {
      return;
    }

    // Load the metadata if it hasn't been loaded yet.
    if (metadata === null) {
      fetch(path)
        .then((response) => {
          return response.json();
        })
        .then((data) => {
          if (data.metadata === undefined || data.metadata === null) {
            setError("Failed to load configuration metadata.");
            throw new Error("Metadata missing meta.");
          } else if (data.values === undefined || data.values === null) {
            setError("Failed to load current configuration.");
            throw new Error("Metadata missing values.");
          } else {
            setMetadata(data.metadata);
            setValues(data.values);
          }
        })
        .catch((error) => {
          console.error("Error attempting to load metadata:\n", error);
        });
    }
  }, [ggsConnectionStatus, metadata, ggsAddress, resource, category, values]);

  function Save() {
    console.log("Saving...");
  }

  function toggleEditMode() {
    if (!editMode) {
      setEditMode(true);
    } else {
      Save();
    }
  }

  if (error !== null) {
    return <>{error}</>;
  } else if (metadata === null || values === null) {
    return <>Loading...</>;
  } else {
    return (
      <>
        <Table columns={metadata} data={values} editmode={editMode} />
        <StyButton
          onClick={toggleEditMode}
          style={{ display: "flex", justifyContent: "center" }}
        >
          {editMode ? "save" : "edit"}
        </StyButton>
      </>
    );
  }
}
