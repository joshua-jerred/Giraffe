var router = require('express').Router();

// gfs/meta
router.get('/meta', function (req, res) {
  const body = req.body;
  if (body['category'] === undefined) {
    res.status(400).json({ error: 'category is required' });
    return;
  }

  const category = body['category'];
  const subcategory =
    body['subcategory'] === undefined ? null : body['subcategory'];

  const meta_data = require('../metadata/metaLoader')(
    'gfs',
    category,
    subcategory
  );

  if (meta_data === null) {
    res.status(400).json({ error: 'invalid category or subcategory' });
    return;
  }

  res.json(meta_data);
});

// gfs/status
router.get('/status', function (req, res) {
  res.json({ status: 'ok' });
});

// gfs/*
router.get('/:resource', function (req, res) {
  const allowedResources = ['general', 'debugging', 'telemetry'];
  console.log(req.path, req.params);
  res.json({ status: 'ok' });
});

module.exports = router;
