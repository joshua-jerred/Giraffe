import {
  PageTitle,
  PageContent,
  Card,
  CardRow,
  CardMasonryLayout,
} from "../../core/PageParts";

import { useEffect, useState } from "react";

import { Markdown } from "../../components/markdown";

import Acronyms from "../../assets/docs/acronyms.md";
// import Commands from '../../assets/docs/commands.md';
// import Errors from '../../assets/docs/errors.md';
// import Versions from '../../assets/docs/change_log.md';
import Attribution from "../../assets/docs/attribution.md";
import { GiraffeVersion } from "giraffe-protocol";

function DocsPage() {
  const [acronyms, setAcronyms] = useState("");
  const [commands, setCommands] = useState("");
  const [errors, setErrors] = useState("");
  const [versions, setVersions] = useState("");
  const [attribution, setAttribution] = useState("");

  useEffect(() => {
    fetch(Acronyms)
      .then((response) => response.text())
      .then((data) => {
        setAcronyms(data);
      });

    // fetch(Commands)
    //   .then((response) => response.text())
    //   .then((data) => {
    //     setCommands(data);
    //   });

    // fetch(Errors)
    //   .then((response) => response.text())
    //   .then((data) => {
    //     setErrors(data);
    //   });

    // fetch(Versions)
    //   .then((response) => response.text())
    //   .then((data) => {
    //     setVersions(data);
    //   });

    fetch(Attribution)
      .then((response) => response.text())
      .then((data) => {
        setAttribution(data);
      });
  }, []);

  return (
    <>
      <PageTitle>Giraffe Documentation</PageTitle>
      <PageContent>
        <CardMasonryLayout>
          <Card title="version info">
            Giraffe Version: {GiraffeVersion.version_string}
            <br />
            Build Date and Time: {GiraffeVersion.build_date}{" "}
            {GiraffeVersion.build_time}
            <br />
            git: {GiraffeVersion.git_hash} ({GiraffeVersion.git_branch} /{" "}
            {GiraffeVersion.git_commit_count})
            <br />
            {GiraffeVersion.git_repo}
            <br />
            NodeJS Version: {GiraffeVersion.nodejs_version}
          </Card>
          <Card title="attribution">
            <Markdown children={attribution} />
          </Card>
          <Card title="acronyms">
            <Markdown children={acronyms} />
          </Card>
          <Card title="commands">
            <Markdown children={commands} />
          </Card>
          <Card title="errors">
            <Markdown children={errors} />
          </Card>
        </CardMasonryLayout>
      </PageContent>
    </>
  );
}

export default DocsPage;
